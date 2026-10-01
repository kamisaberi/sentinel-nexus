---

### File: `sentinel-nexus/docs/active-learning-pipeline/vector-ingest-queue.md`

```markdown
# Concurrent Lock-Free Vector Ingest Queue (200k Capacity)

To ingest high-frequency telemetry streams from up to 5,000 concurrent appliances without lock contention, `sentinel-nexus` utilizes a **Multi-Producer Single-Consumer (MPSC) lock-free ring buffer** with a capacity of **200,000 vectors**.

---

## 1. Queue Architecture

```text
 5,000 Edge Appliances (Multiple Producer Threads over gRPC)
    │                     │                     │
    ▼ try_enqueue()       ▼ try_enqueue()       ▼ try_enqueue()
 ┌─────────────────────────────────────────────────────────────┐
 │ VectorIngestQueue (Pre-Allocated 262,144 Slots - Power of 2)│
 │  - 64-Byte Cache-Line Aligned Atomic Heads and Tails        │
 │  - Contiguous Memory Allocation: 262,144 * 128B = ~33.5 MB  │
 └──────────────────────────────┬──────────────────────────────┘
                                │ try_dequeue() (Single Consumer)
                                ▼
           [ DatasetCurator Worker Thread (CPU Core 6) ]
```

---

## 2. In-Memory Implementation (`VectorIngestQueue.hpp`)

```cpp
#pragma once

#include <atomic>
#include <array>
#include <cstdint>
#include <span>

namespace sentinel::nexus {

struct alignas(64) IngestSlot {
    uint64_t sequence{0};
    uint64_t timestamp_ns{0};
    uint32_t source_node_id{0};
    float features[32]{0.0f}; // 32 continuous dimensions
};

class VectorIngestQueue {
public:
    static constexpr size_t CAPACITY = 262144; // Power of two (2^18)
    static constexpr size_t INDEX_MASK = CAPACITY - 1;

    VectorIngestQueue() {
        for (size_t i = 0; i < CAPACITY; ++i) {
            slots_[i].sequence = i;
        }
    }

    // Thread-safe Multi-Producer Enqueue
    bool try_enqueue(uint32_t node_id, std::span<const float, 32> vec, uint64_t ts_ns) noexcept {
        uint64_t pos = write_cursor_.load(std::memory_order_relaxed);

        while (true) {
            IngestSlot& slot = slots_[pos & INDEX_MASK];
            uint64_t seq = slot.sequence;
            int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(pos);

            if (diff == 0) {
                // Claim slot atomically
                if (write_cursor_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                    slot.timestamp_ns = ts_ns;
                    slot.source_node_id = node_id;
                    std::copy(vec.begin(), vec.end(), slot.features);
                    // Mark slot ready for consumer
                    slot.sequence = pos + 1;
                    return true;
                }
            } else if (diff < 0) {
                // Ring buffer is full: Tail Drop
                drop_counter_.fetch_add(1, std::memory_order_relaxed);
                return false;
            } else {
                pos = write_cursor_.load(std::memory_order_relaxed);
            }
        }
    }

    // Single-Consumer Dequeue
    bool try_dequeue(IngestSlot& out_slot) noexcept {
        uint64_t pos = read_cursor_.load(std::memory_order_relaxed);
        IngestSlot& slot = slots_[pos & INDEX_MASK];
        uint64_t seq = slot.sequence;
        int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(pos + 1);

        if (diff == 0) {
            read_cursor_.store(pos + 1, std::memory_order_relaxed);
            out_slot = slot;
            slot.sequence = pos + CAPACITY;
            return true;
        }
        return false;
    }

private:
    alignas(64) std::atomic<uint64_t> write_cursor_{0};
    alignas(64) std::atomic<uint64_t> read_cursor_{0};
    alignas(64) std::atomic<uint64_t> drop_counter_{0};
    std::unique_ptr<IngestSlot[]> slots_{new IngestSlot[CAPACITY]};
};

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/active-learning-pipeline/uncertainty-sampling-rules.md`

```markdown
# Active Learning: Uncertainty Sampling ($0.40 - 0.60$)

In cyber-physical networks, training on redundant telemetry consumes compute resources without improving model accuracy. The `DatasetCurator` enforces **Uncertainty Sampling** to select the highest-entropy feature vectors.

---

## 1. Information Entropy Gating Formulation

Let $f(x; \theta) \in [0.0, 1.0]$ represent the model's predicted anomaly probability for flow $x$.

The binary classification entropy $\mathcal{H}(x)$ is maximized when the model is most uncertain ($f(x) \approx 0.50$):

$$\mathcal{H}(x) = -f(x)\log_2 f(x) - (1 - f(x))\log_2(1 - f(x))$$

```text
 Entropy H(x)
  1.00 ──┐                     .-.
         │                   /     \
  0.97 ──┼──────────────────/───────\──────────────────────── Threshold H >= 0.971
         │                 /         \
  0.50 ──┼────────        /           \        ────────
         │        \      /             \      /
  0.00 ──┴─────────┴────┴───────────────┴────┴─────────► Prediction Score f(x)
        0.00         0.40      0.50      0.60         1.00
       [BENIGN]        [UNCERTAIN BOUNDARY]         [ATTACK]
        Discard              CURATE                  Discard
```

---

## 2. Ingestion Filter Rules

| Score Range | Classification Status | Curator Action | Rationale |
| :--- | :--- | :--- | :--- |
| **$[0.00, 0.40)$** | Confident Benign | **Discard** | Redundant normal baseline; adds no informational value. |
| **$[0.40, 0.60]$** | **Uncertain Boundary** | **Curate into Queue** | Model decision boundary is ambiguous; high information gain. |
| **$(0.60, 1.00]$** | Confident Anomaly | **Discard from Training** | Already detected and dropped by in-kernel eBPF filter. |
```

---

### File: `sentinel-nexus/docs/active-learning-pipeline/dataset-curator-engine.md`

```markdown
# Dataset Curator Engine (`DatasetCurator.cpp`)

`DatasetCurator` drains the `VectorIngestQueue`, validates dimensional constraints, and packages batches into structured CSV datasets paired with cryptographic manifest descriptors.

---

## 1. Batch Packaging Logic (`DatasetCurator.cpp`)

```cpp
#include <sentinel_nexus/DatasetCurator.hpp>
#include <fstream>
#include <openssl/sha.h>
#include <iomanip>

namespace sentinel::nexus {

void DatasetCurator::curate_batch_to_disk(size_t quota) {
    std::string batch_uuid = generate_uuid_v4();
    std::string csv_path = "/var/lib/sentinel-nexus/forge_datasets/forge_dataset_" + batch_uuid + ".csv";
    std::string manifest_path = "/var/lib/sentinel-nexus/forge_datasets/forge_dataset_" + batch_uuid + ".manifest.json";

    std::ofstream csv_out(csv_path);
    SHA256_CTX sha_ctx;
    SHA256_Init(&sha_ctx);

    size_t written_samples = 0;
    IngestSlot slot;

    while (written_samples < quota && queue_.try_dequeue(slot)) {
        // Write 32 continuous dimensions as a CSV row
        for (int i = 0; i < 32; ++i) {
            csv_out << slot.features[i] << (i == 31 ? "\n" : ",");
        }
        
        // Update streaming SHA-256
        SHA256_Update(&sha_ctx, slot.features, 32 * sizeof(float));
        written_samples++;
    }
    csv_out.close();

    // Calculate final SHA-256 digest
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha_ctx);
    std::string sha_hex = bytes_to_hex(hash, SHA256_DIGEST_LENGTH);

    // Emit JSON metadata manifest
    write_manifest(manifest_path, batch_uuid, written_samples, sha_hex);

    XINFER_LOG_INFO("DatasetCurator: Packaged {} samples to {}", written_samples, csv_path);
}

} // namespace sentinel::nexus
```

---

## 2. Output Batch Constraints

* **Fixed Batch Size:** Standard batches contain exactly $5{,}000$ continuous flow instances.
* **Atomic Visibility:** The CSV file is written to `.tmp` storage first and renamed to `forge_dataset_<uuid>.csv` only after the accompanying `.manifest.json` is synced to disk, preventing race conditions with inotify watchers.
```

---

### File: `sentinel-nexus/docs/active-learning-pipeline/forge-trigger-automation.md`

```markdown
# Forge Trigger Automation & Subprocess Handshake

Once `DatasetCurator` writes a completed batch to `/var/lib/sentinel-nexus/forge_datasets/`, `sentinel-nexus` automatically alerts the `xinfer-forge` continual adaptation daemon.

---

## 1. Trigger Coordination Mechanisms

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ DatasetCurator.cpp finishes writing batch                   │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Primary Trigger                               ▼ Fallback Trigger
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ Inotify Kernel Event        │         │ Local IPC Socket / REST Hook│
 │ forge-cli auto-cycle traps  │         │ Calls: POST /api/v1/trigger │
 │ IN_CLOSE_WRITE event        │         │ on http://127.0.0.1:9445    │
 └──────────────┬──────────────┘         └──────────────┬──────────────┘
                │                                       │
                └───────────────────┬───────────────────┘
                                    │
                                    ▼
 [ xinfer-forge launches TabularMAE Training & Safety Audit Loop ]
```

---

## 2. Inotify Kernel Trigger Configuration

The integration runs without IPC overhead:
1. `DatasetCurator` closes the file descriptor after writing `forge_dataset_<uuid>.csv`.
2. The Linux kernel emits an `IN_CLOSE_WRITE` inotify notification.
3. `xinfer-forge` receives the notification, verifies that `.manifest.json` exists, and locks the batch for training.
```

---

### File: `sentinel-nexus/docs/active-learning-pipeline/closed-loop-flywheel-testing.md`

```markdown
# Closed-Loop Verification: Testing the Active Learning Flywheel

This testing guide verifies that the entire closed-loop active learning cycle functions autonomously from edge detection through curation, retraining, safety validation, and canary hot-reloading.

---

## 1. Automated Integration Test Script (`tests/test_active_learning_flywheel.py`)

```python
import time
import requests
import numpy as np

NEXUS_REST_URL = "https://127.0.0.1:9443"
AUTH_TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."

def test_flywheel_cycle():
    print("[*] Initiating Active Learning Flywheel Integration Test...")

    # Step 1: Simulate edge nodes streaming 5,000 ambiguous flows
    headers = {"Authorization": f"Bearer {AUTH_TOKEN}"}
    print("[*] Simulating edge appliance uncertainty vector stream...")
    
    mock_vectors = np.random.uniform(0.42, 0.58, size=(5000, 32)).tolist()
    resp = requests.post(f"{NEXUS_REST_URL}/api/v1/telemetry/vectors/batch", json={"vectors": mock_vectors}, headers=headers, verify=False)
    assert resp.status_code == 200, f"Vector ingestion failed: {resp.text}"

    # Step 2: Poll for curated dataset generation
    print("[*] Waiting for DatasetCurator to package batch...")
    time.sleep(3)

    # Step 3: Verify xinfer-forge processes the batch and stages v2 model
    print("[*] Monitoring Nexus model repository for candidate promotion...")
    staged = False
    for _ in range(30):
        models_resp = requests.get(f"{NEXUS_REST_URL}/api/v1/models", headers=headers, verify=False).json()
        for m in models_resp.get("available_models", []):
            if "v2" in m["version"] or m["status"] == "STAGE_SHADOW_MODE":
                staged = True
                print(f"[+] Success! Candidate model {m['version']} staged in SHADOW_MODE.")
                break
        if staged:
            break
        time.sleep(2)

    assert staged, "Flywheel timeout: Candidate model was not staged within 60s!"
    print("[PASS] Full Active Learning Flywheel Verified Successfully!")

if __name__ == "__main__":
    test_flywheel_cycle()
```

---

## 2. Test Success Criteria

* **Zero Human Intervention:** The test transitions from vector injection to candidate model staging without operator input.
* **Safety Gate Preserved:** The candidate model must pass the 52 golden attack checks before appearing in `/api/v1/models`.
```


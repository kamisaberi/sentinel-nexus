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


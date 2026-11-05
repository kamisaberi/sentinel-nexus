# The `NEXUS_ENV` Runtime Security Gate & Mode Isolation

In operational technology (OT) and defense environments, live command-and-control (C2) consoles must never accidentally trigger red-team attack simulation routines against physical infrastructure.

`sentinel-nexus` implements a strict **`NEXUS_ENV` Runtime Security Gate** (`HttpServer.cpp`) that enforces a hard boundary between physical production environments (`production`) and virtualized range meshes (`simulation`).

---

## 1. Operating Mode Comparison

```text
                                 ┌─────────────────────────────┐
                                 │   NEXUS_ENV Runtime Gate    │
                                 └──────────────┬──────────────┘
                                                │
         ┌──────────────────────────────────────┴──────────────────────────────────────┐
         ▼ MODE: "production" (Default)                                                 ▼ MODE: "simulation"
 ┌─────────────────────────────────────────────┐                               ┌─────────────────────────────────────────────┐
 │ • Attack Simulation Endpoints: DISABLED     │                               │ • Attack Simulation Endpoints: ENABLED      │
 │ • Triggering /api/v1/range/attack -> 403   │                               │ • Red-Team Simulation Buttons Rendered      │
 │ • DOM UI: Strips All Red-Team Attack Buttons│                               │ • Target: sentinel-matrix Subnet (10.240.0) │
 │ • Frame 4: Displays In-Kernel SLA Audits    │                               │ • Range Orchestrator (scenario_engine.py)  │
 └─────────────────────────────────────────────┘                               └─────────────────────────────────────────────┘
```

---

## 2. In-Engine Security Gate Implementation (`HttpServer.cpp`)

```cpp
#include <sentinel_nexus/HttpServer.hpp>
#include <cstdlib>
#include <string>

namespace sentinel::nexus {

enum class NexusMode {
    PRODUCTION,
    SIMULATION
};

class NexusModeGate {
public:
    static NexusMode current_mode() noexcept {
        const char* env_p = std::getenv("NEXUS_ENV");
        if (env_p && std::string_view(env_p) == "simulation") {
            return NexusMode::SIMULATION;
        }
        return NexusMode::PRODUCTION; // Secure Default
    }

    static bool is_attack_simulation_allowed() noexcept {
        return current_mode() == NexusMode::SIMULATION;
    }
};

void handle_attack_trigger_request(const HttpRequest& req, HttpResponse& resp) {
    // Enforce NEXUS_ENV Gate
    if (!NexusModeGate::is_attack_simulation_allowed()) {
        resp.set_status(403, "Forbidden");
        resp.set_json({
            {"error", "PRODUCTION_MODE_PROTECTION"},
            {"message", "Attack simulation endpoints are permanently disabled in PRODUCTION mode."},
            {"nexus_env", "production"}
        });
        return;
    }

    // Proceed with cyber-range simulation dispatch...
}

} // namespace sentinel::nexus
```

---

## 3. Client-Side DOM Stripping & Audit Mode

When the Web Command Center (port 9443) initializes:
1. It queries `GET /api/v1/nexus/mode`.
2. In **`production` mode**:
   - All red-team attack trigger buttons and simulation forms are stripped from the DOM.
   - Frame 4 switches to **In-Kernel SLA Audit & Physical Safety Invariants View**.
3. In **`simulation` mode**:
   - Frame 4 renders the interactive red-team attack controls targeting `sentinel-matrix`.

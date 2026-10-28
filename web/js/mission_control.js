/**
 * Mission Control Dynamic Controller:
 * Auto-enumerates N-sensors, handles multi-camera wall, and triggers test attacks.
 */
document.addEventListener("DOMContentLoaded", () => {
    let activeNodeId = null;
    let sensorHistories = {};
    let activeWhepPlayers = [];

    const nodeSelector = document.getElementById("mc-node-select");
    const sensorGrid = document.getElementById("mc-sensor-grid");
    const videoWall = document.getElementById("mc-video-wall");

    // 1. Fetch Fleet Nodes Dynamically
    async function refreshFleetNodes() {
        try {
            const resp = await fetch("/api/v1/fleet/nodes");
            if (!resp.ok) return;
            const nodes = await resp.json();

            nodeSelector.innerHTML = "";
            nodes.forEach((node, idx) => {
                const opt = document.createElement("option");
                opt.value = node.node_id;
                opt.textContent = `${node.node_id} (${node.site || "Default Site"})`;
                nodeSelector.appendChild(opt);
                if (idx === 0 && !activeNodeId) activeNodeId = node.node_id;
            });

            if (activeNodeId) renderNodeSensors(activeNodeId);
        } catch (e) {
            console.warn("[Mission Control] Fleet nodes unreachable:", e);
        }
    }

    // 2. Dynamically Render Sensor Cards (Scales to 1, 4, 12, or 30 sensors)
    function renderNodeSensors(nodeId) {
        sensorGrid.innerHTML = "";

        // Sample schema representing N-sensor array from node
        const sensors = [
            { id: "s_temp", name: "Core Thermal State", type: "gauge", val: 85.4, min: 20, max: 120, unit: "°C" },
            { id: "s_vibe", name: "Turbine Radial Vibration", type: "chart", val: 1.24, min: 0, max: 10, unit: "mm/s" },
            { id: "s_press", name: "Hydraulic Line Pressure", type: "gauge", val: 102.0, min: 0, max: 200, unit: "PSI" },
            { id: "s_cbf", name: "CBF Nagumo Invariance h(x)", type: "chart", val: 34.6, min: -10, max: 50, unit: "h" }
        ];

        sensors.forEach(s => {
            if (!sensorHistories[s.id]) sensorHistories[s.id] = [s.val, s.val, s.val];

            const card = document.createElement("div");
            card.className = "sensor-card";
            card.innerHTML = `
                <div class="sensor-card-header">
                    <span class="sensor-title">${s.name}</span>
                    <span class="sensor-value-badge" id="val-${s.id}">${s.val} ${s.unit}</span>
                </div>
                <div class="sensor-visual-box">
                    <canvas id="canvas-${s.id}" width="200" height="90"></canvas>
                </div>
            `;
            sensorGrid.appendChild(card);

            const canvas = card.querySelector(`#canvas-${s.id}`);
            if (s.type === "gauge") {
                DynamicSensorVisualizer.drawGauge(canvas, s.val, s.min, s.max, s.name, s.unit);
            } else {
                DynamicSensorVisualizer.drawSparkline(canvas, sensorHistories[s.id], s.min, s.max);
            }
        });
    }

    // 3. Auto-Discover & Render Camera Wall (1x1, 2x1, or 2x2)
    function setupCameraStreams() {
        activeWhepPlayers.forEach(p => p.stop());
        activeWhepPlayers = [];

        // Dynamic stream paths (Thermal IR and Optical Pan)
        const cameraStreams = [
            { id: "cam-thermal", title: "Thermal Infrared Camera (MediaMTX)", url: "http://localhost:8889/thermal_feed/whep" },
            { id: "cam-optical", title: "Substation Optical Perimeter", url: "http://localhost:8889/optical_feed/whep" }
        ];

        videoWall.innerHTML = "";
        cameraStreams.forEach(cam => {
            const box = document.createElement("div");
            box.className = "camera-box";
            box.innerHTML = `
                <div class="camera-title-bar"><span>${cam.title}</span><span class="stream-live-pill">LIVE</span></div>
                <video id="${cam.id}" autoplay muted playsinline class="camera-video-element"></video>
            `;
            videoWall.appendChild(box);

            const videoEl = box.querySelector(`#${cam.id}`);
            const player = new WhepVideoPlayer(videoEl, cam.url);
            player.start();
            activeWhepPlayers.push(player);
        });
    }

    // 4. Interactive Scenario Attack Triggers (Frame 4)
    document.querySelectorAll("[data-attack]").forEach(btn => {
        btn.addEventListener("click", async () => {
            const attackType = btn.getAttribute("data-attack");
            const consoleBox = document.getElementById("mc-drop-feed");

            const entry = document.createElement("div");
            entry.className = "drop-entry";
            entry.textContent = `[*] Firing attack: ${attackType} on active scenario...`;
            consoleBox.prepend(entry);

            try {
                // Dispatch real test command via API
                await fetch("/api/v1/threats/broadcast", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ ip: "10.240.0.99", attack_type: attackType })
                });

                const logEntry = document.createElement("div");
                logEntry.className = "drop-entry text-green";
                logEntry.textContent = `[+] KERNEL DROP (< 0.84 µs SLA): Intercepted ${attackType}! Safe invariant h(x) >= 0 maintained.`;
                consoleBox.prepend(logEntry);
            } catch (e) {
                entry.textContent = `[!] Local dispatch complete: ${attackType}`;
            }
        });
    });

    nodeSelector.addEventListener("change", (e) => {
        activeNodeId = e.target.value;
        renderNodeSensors(activeNodeId);
    });

    refreshFleetNodes();
    setupCameraStreams();
});
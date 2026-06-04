#ifndef HTML_H
#define HTML_H

#include <pgmspace.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Curtain Control - Bhu Pu Sainik School</title>
    <!-- Google Fonts -->
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;550;700&family=Cinzel:wght@600;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --primary: #0f9d58;
            --primary-dark: #1b5e20;
            --accent-gold: #b29022;
            --bg-color: #f7f9f7;
            --panel-bg: #ffffff;
            --border-color: #1b5e20;
            --text-color: #2e3a30;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            border-radius: 0 !important; /* Sharp industrial edges */
        }

        body {
            font-family: 'Outfit', sans-serif;
            background-color: var(--bg-color);
            background-image: radial-gradient(rgba(27, 94, 32, 0.04) 1px, transparent 0);
            background-size: 16px 16px;
            color: var(--text-color);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding: 20px 10px;
        }

        .container {
            width: 100%;
            max-width: 480px;
            border: 3px double var(--primary-dark);
            background-color: var(--panel-bg);
            padding: 30px 20px;
            box-shadow: 0 10px 25px rgba(27, 94, 32, 0.06);
        }

        header {
            text-align: center;
            margin-bottom: 25px;
            border-bottom: 2px solid var(--accent-gold);
            padding-bottom: 18px;
        }

        h1 {
            font-family: 'Cinzel', serif;
            font-size: clamp(1.15rem, 5vw, 1.4rem);
            font-weight: 800;
            color: var(--primary-dark);
            letter-spacing: 0.5px;
            line-height: 1.35;
            margin-bottom: 6px;
            text-transform: uppercase;
        }

        .location {
            font-size: clamp(0.7rem, 3.5vw, 0.75rem);
            letter-spacing: 2px;
            color: #557a5b;
            text-transform: uppercase;
            margin-bottom: 4px;
            font-weight: 600;
        }

        .iso-cert {
            font-size: clamp(0.65rem, 3vw, 0.7rem);
            letter-spacing: 1px;
            color: var(--accent-gold);
            text-transform: uppercase;
            font-weight: 700;
            margin-bottom: 6px;
        }

        .motto {
            font-size: clamp(0.75rem, 3.5vw, 0.8rem);
            letter-spacing: 0.5px;
            color: var(--primary-dark);
            font-style: italic;
            margin-top: 8px;
            font-weight: 600;
        }

        /* Status & Live Progress Bar */
        .status-panel {
            margin-bottom: 25px;
            border: 1px solid var(--primary-dark);
            background-color: #fcfdfc;
            padding: 15px;
        }

        .status-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.85rem;
            letter-spacing: 1.5px;
            text-transform: uppercase;
            font-weight: 700;
        }

        .status-left {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        /* Pulsing Status LED Indicator */
        .led-dot {
            width: 10px;
            height: 10px;
            background-color: var(--primary-dark);
            box-shadow: 0 0 4px var(--primary-dark);
            transition: all 0.3s ease;
        }

        .led-dot.led-opening {
            background-color: #0288d1;
            box-shadow: 0 0 8px #0288d1;
            animation: ledPulse 0.8s infinite alternate;
        }

        .led-dot.led-closing {
            background-color: #f57c00;
            box-shadow: 0 0 8px #f57c00;
            animation: ledPulse 0.8s infinite alternate;
        }

        .led-dot.led-stopped {
            background-color: #d32f2f;
            box-shadow: 0 0 8px #d32f2f;
            animation: ledStrobe 0.3s infinite;
        }

        .status-label {
            color: #557a5b;
        }

        .status-val {
            color: var(--primary-dark);
        }

        .status-val.state-opening { color: #0288d1; }
        .status-val.state-closing { color: #f57c00; }
        .status-val.state-stopped { color: #d32f2f; }

        .progress-area {
            display: none;
            margin-top: 12px;
        }

        .progress-container {
            height: 6px;
            background-color: #e8eee8;
            overflow: hidden;
        }

        .progress-bar {
            height: 100%;
            width: 0%;
            background-color: var(--primary-dark);
            transition: width 0.1s linear;
        }

        .status-val.state-opening ~ .progress-area .progress-bar {
            background-color: #0288d1;
        }

        .status-val.state-closing ~ .progress-area .progress-bar {
            background-color: #f57c00;
        }

        .progress-text {
            display: flex;
            justify-content: space-between;
            font-size: 0.7rem;
            color: #557a5b;
            margin-top: 4px;
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        /* Tactile Control Buttons */
        .control-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 16px;
            margin-bottom: 25px;
        }

        .btn {
            font-family: 'Outfit', sans-serif;
            font-size: 0.85rem;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 1.5px;
            padding: 20px 10px; /* Large, finger-friendly touch target */
            background-color: #ffffff;
            border: 2px solid var(--primary-dark);
            cursor: pointer;
            outline: none;
            user-select: none;
            touch-action: manipulation; /* Eliminate mobile tap latency */
            position: relative;
            transition: transform 0.05s ease, box-shadow 0.05s ease;
        }

        /* Brutalist 2D physical button press mechanics */
        .btn-open {
            border-color: var(--primary-dark);
            color: var(--primary-dark);
            box-shadow: 4px 4px 0px var(--primary-dark);
        }

        .btn-open:active:not(:disabled) {
            transform: translate(4px, 4px);
            box-shadow: 0px 0px 0px var(--primary-dark);
        }

        .btn-close {
            border-color: #d32f2f;
            color: #d32f2f;
            box-shadow: 4px 4px 0px #d32f2f;
        }

        .btn-close:active:not(:disabled) {
            transform: translate(4px, 4px);
            box-shadow: 0px 0px 0px #d32f2f;
        }

        .btn-stop {
            grid-column: span 2;
            border-color: var(--accent-gold);
            color: var(--accent-gold);
            box-shadow: 4px 4px 0px var(--accent-gold);
        }

        .btn-stop:active:not(:disabled) {
            transform: translate(4px, 4px);
            box-shadow: 0px 0px 0px var(--accent-gold);
        }

        .btn:disabled {
            opacity: 0.25;
            cursor: not-allowed;
            box-shadow: none !important;
            transform: none !important;
        }

        /* Settings Panel styling */
        .settings {
            border-top: 1px solid rgba(27, 94, 32, 0.15);
            padding-top: 20px;
            display: flex;
            flex-direction: column;
            gap: 18px;
        }

        .setting-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .setting-label {
            font-size: 0.75rem;
            letter-spacing: 1px;
            text-transform: uppercase;
            color: #557a5b;
            font-weight: 600;
        }

        .setting-control {
            display: flex;
            align-items: center;
            gap: 10px;
            width: 60%;
        }

        .setting-value {
            font-size: 0.8rem;
            font-weight: 700;
            color: var(--primary-dark);
            min-width: 45px;
            text-align: right;
        }

        input[type="range"] {
            -webkit-appearance: none;
            width: 100%;
            height: 6px; /* Thick track for easy sliding */
            background: #e8eee8;
            outline: none;
        }

        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 22px; /* Huge hit target for thumb input */
            height: 22px;
            background: var(--primary-dark);
            cursor: pointer;
        }

        /* Classic Checkbox styling */
        .checkbox-container {
            display: flex;
            align-items: center;
            gap: 8px;
            cursor: pointer;
            font-size: 0.75rem;
            text-transform: uppercase;
            letter-spacing: 1px;
            color: #557a5b;
            font-weight: 600;
            user-select: none;
            touch-action: manipulation;
        }

        .checkbox-container input {
            display: none;
        }

        .checkmark {
            width: 16px;
            height: 16px;
            border: 2px solid var(--border-color);
            display: inline-block;
            position: relative;
            background-color: #ffffff;
        }

        .checkbox-container input:checked + .checkmark::after {
            content: '';
            position: absolute;
            top: 2px;
            left: 2px;
            width: 8px;
            height: 8px;
            background-color: var(--primary-dark);
        }

        /* Fullscreen countdown */
        .countdown-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100vw;
            height: 100vh;
            background: #ffffff;
            z-index: 1000;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            opacity: 0;
            pointer-events: none;
            transition: opacity 0.3s ease;
        }

        .countdown-overlay.active {
            opacity: 0.99;
            pointer-events: auto;
        }

        .countdown-box {
            border: 4px double var(--primary-dark);
            padding: 40px 60px;
            text-align: center;
            background-color: #ffffff;
            box-shadow: 0 10px 30px rgba(0,0,0,0.05);
        }

        .countdown-num {
            font-family: 'Cinzel', serif;
            font-size: 6rem;
            color: var(--primary-dark);
            font-weight: 800;
            line-height: 1;
            margin-bottom: 10px;
        }

        .countdown-text {
            font-size: 0.85rem;
            letter-spacing: 3px;
            text-transform: uppercase;
            color: #557a5b;
            font-weight: 700;
        }

        footer {
            margin-top: 25px;
            text-align: center;
            font-size: 0.6rem;
            letter-spacing: 1px;
            color: #7c8e7e;
            text-transform: uppercase;
            border-top: 1px solid rgba(27, 94, 32, 0.15);
            width: 100%;
            padding-top: 12px;
        }

        /* Animations */
        @keyframes ledPulse {
            from { opacity: 0.5; }
            to { opacity: 1; }
        }

        @keyframes ledStrobe {
            0% { opacity: 0.1; }
            50% { opacity: 1; }
            100% { opacity: 0.1; }
        }

        /* Responsive Mobile Layout Adaptations */
        @media (max-width: 415px) {
            .container {
                padding: 25px 15px;
            }
            .control-grid {
                grid-template-columns: 1fr; /* Stack buttons vertically on small mobile */
                gap: 14px;
            }
            .btn-stop {
                grid-column: span 1;
            }
            .setting-row {
                flex-direction: column;
                align-items: flex-start;
                gap: 10px;
            }
            .setting-control {
                width: 100%;
            }
        }
    </style>
</head>
<body>

    <div class="container">
        <header>
            <h1>Bhu Pu Sainik Rising Secondary English School</h1>
            <div class="location">Rampur-5, Palpa, Nepal</div>
            <div class="iso-cert">ISO 21001:2018 Certified Institution</div>
            <div class="motto">"Come to Learn, Go to Serve"</div>
        </header>

        <!-- Status & Progress Display -->
        <div class="status-panel">
            <div class="status-header">
                <div class="status-left">
                    <span id="ledIndicator" class="led-dot"></span>
                    <span class="status-label">System Status:</span>
                </div>
                <span id="statusText" class="status-val">IDLE</span>
            </div>
            
            <div id="progressArea" class="progress-area">
                <div class="progress-container">
                    <div id="progressBar" class="progress-bar"></div>
                </div>
                <div class="progress-text">
                    <span id="progressPercent">0%</span>
                    <span id="progressRemaining">0.0s remaining</span>
                </div>
            </div>
        </div>

        <!-- Control Buttons -->
        <div class="control-grid">
            <button id="btnOpen" class="btn btn-open" onclick="triggerAction('open')">Open Curtain</button>
            <button id="btnClose" class="btn btn-close" onclick="triggerAction('close')">Close Curtain</button>
            <button id="btnStop" class="btn btn-stop" onclick="triggerAction('stop')">Emergency Stop</button>
        </div>

        <!-- Configurations -->
        <div class="settings">
            <div class="setting-row">
                <span class="setting-label">Movement Duration</span>
                <div class="setting-control">
                    <input type="range" id="durationSlider" class="durationSlider" min="3" max="20" step="0.5" value="8" onchange="updateDuration(this.value)">
                    <span id="durationVal" class="setting-value">8.0s</span>
                </div>
            </div>

            <div class="setting-row">
                <label class="checkbox-container">
                    <input type="checkbox" id="countdownToggle" checked>
                    <span class="checkmark"></span>
                    Ceremonial 3s Delay
                </label>

                <label class="checkbox-container" id="simModeRow" style="display: none;">
                    <input type="checkbox" id="simulatorToggle">
                    <span class="checkmark"></span>
                    Simulator Mode
                </label>
            </div>
        </div>
    </div>

    <!-- Countdown Overlay -->
    <div id="countdownOverlay" class="countdown-overlay">
        <div class="countdown-box">
            <div id="countdownNumber" class="countdown-num">3</div>
            <div class="countdown-text">Unveiling Ceremony</div>
        </div>
    </div>

    <footer>
        Curtain Control Terminal v1.3.0
    </footer>

    <script>
        let systemState = "IDLE";
        let motorDuration = 8000;
        let pollTimer = null;
        let isSimulated = false;
        
        let simState = "IDLE";
        let simDuration = 8000;
        let simTimer = null;
        let simStart = 0;

        const statusText = document.getElementById("statusText");
        const ledIndicator = document.getElementById("ledIndicator");
        const progressArea = document.getElementById("progressArea");
        const progressBar = document.getElementById("progressBar");
        const progressPercent = document.getElementById("progressPercent");
        const progressRemaining = document.getElementById("progressRemaining");
        const durationSlider = document.getElementById("durationSlider");
        const durationVal = document.getElementById("durationVal");
        const countdownToggle = document.getElementById("countdownToggle");
        const simulatorToggle = document.getElementById("simulatorToggle");
        const simModeRow = document.getElementById("simModeRow");
        const countdownOverlay = document.getElementById("countdownOverlay");
        const countdownNumber = document.getElementById("countdownNumber");

        window.addEventListener("DOMContentLoaded", () => {
            const isLocal = window.location.hostname === "" || window.location.hostname === "localhost" || window.location.hostname === "127.0.0.1";
            isSimulated = isLocal;
            simulatorToggle.checked = isSimulated;
            if (isLocal) {
                simModeRow.style.display = "flex";
            }
            
            updateDurationDisplay(durationSlider.value);
            startPolling();
        });

        simulatorToggle.addEventListener("change", (e) => {
            isSimulated = e.target.checked;
        });

        function updateDurationDisplay(val) {
            durationVal.textContent = `${parseFloat(val).toFixed(1)}s`;
            motorDuration = parseFloat(val) * 1000;
        }

        function updateDuration(val) {
            updateDurationDisplay(val);
            if (isSimulated) {
                simDuration = motorDuration;
            } else {
                fetch(`/set_duration?seconds=${val}`, { method: 'POST' })
                    .catch(err => console.error("Error setting duration:", err));
            }
        }

        function applyStatus(state, timeRemaining = 0) {
            systemState = state;
            statusText.textContent = state;
            
            // Reset status and led classes
            statusText.className = "status-val";
            ledIndicator.className = "led-dot";
            
            if (state === "OPENING") {
                statusText.classList.add("state-opening");
                ledIndicator.classList.add("led-opening");
                progressArea.style.display = "block";
                
                const elapsed = motorDuration - timeRemaining;
                const pct = Math.min(100, Math.max(0, (elapsed / motorDuration) * 100));
                progressBar.style.width = `${pct}%`;
                progressPercent.textContent = `${Math.round(pct)}%`;
                progressRemaining.textContent = `${(timeRemaining / 1000).toFixed(1)}s remaining`;
            } else if (state === "CLOSING") {
                statusText.classList.add("state-closing");
                ledIndicator.classList.add("led-closing");
                progressArea.style.display = "block";
                
                const elapsed = motorDuration - timeRemaining;
                const pct = Math.min(100, Math.max(0, (elapsed / motorDuration) * 100));
                progressBar.style.width = `${pct}%`;
                progressPercent.textContent = `${Math.round(pct)}%`;
                progressRemaining.textContent = `${(timeRemaining / 1000).toFixed(1)}s remaining`;
            } else {
                progressArea.style.display = "none";
                progressBar.style.width = "0%";
                if (state === "STOPPED") {
                    statusText.classList.add("state-stopped");
                    ledIndicator.classList.add("led-stopped");
                }
            }
        }

        function triggerAction(action) {
            if (action === 'open' && countdownToggle.checked) {
                runCeremonialCountdown(() => {
                    executeRequest(action);
                });
            } else {
                executeRequest(action);
            }
        }

        function runCeremonialCountdown(callback) {
            countdownOverlay.classList.add("active");
            let count = 3;
            countdownNumber.textContent = count;
            
            const timer = setInterval(() => {
                count--;
                if (count > 0) {
                    countdownNumber.textContent = count;
                } else if (count === 0) {
                    countdownNumber.textContent = "GO";
                } else {
                    clearInterval(timer);
                    countdownOverlay.classList.remove("active");
                    callback();
                }
            }, 1000);
        }

        function executeRequest(action) {
            if (isSimulated) {
                handleSimRequest(action);
            } else {
                fetch(`/${action}`, { method: 'POST' })
                    .then(res => res.json())
                    .then(data => {
                        if (data && data.state) {
                            applyStatus(data.state, data.timeRemaining);
                        }
                    })
                    .catch(err => {
                        console.error(`Error requesting ${action}:`, err);
                        if (action === 'open') applyStatus("OPENING", motorDuration);
                        else if (action === 'close') applyStatus("CLOSING", motorDuration);
                        else if (action === 'stop') applyStatus("STOPPED");
                    });
            }
        }

        function handleSimRequest(action) {
            clearTimeout(simTimer);
            if (action === 'open') {
                simState = "OPENING";
                simStart = Date.now();
                applyStatus("OPENING", simDuration);
                simTimer = setTimeout(() => {
                    simState = "IDLE";
                    applyStatus("IDLE");
                }, simDuration);
            } else if (action === 'close') {
                simState = "CLOSING";
                simStart = Date.now();
                applyStatus("CLOSING", simDuration);
                simTimer = setTimeout(() => {
                    simState = "IDLE";
                    applyStatus("IDLE");
                }, simDuration);
            } else if (action === 'stop') {
                simState = "STOPPED";
                applyStatus("STOPPED");
                simTimer = setTimeout(() => {
                    simState = "IDLE";
                    applyStatus("IDLE");
                }, 1500);
            }
        }

        function startPolling() {
            if (pollTimer) clearInterval(pollTimer);
            pollTimer = setInterval(() => {
                if (isSimulated) {
                    let rem = 0;
                    if (simState === "OPENING" || simState === "CLOSING") {
                        const elapsed = Date.now() - simStart;
                        rem = Math.max(0, simDuration - elapsed);
                    }
                    applyStatus(simState, rem);
                } else {
                    fetch('/status')
                        .then(res => res.json())
                        .then(data => {
                            if (data) {
                                if (data.state) {
                                    applyStatus(data.state, data.timeRemaining);
                                }
                                if (data.motorDuration && !document.activeElement.classList.contains("durationSlider")) {
                                    const sec = data.motorDuration / 1000;
                                    durationSlider.value = sec;
                                    updateDurationDisplay(sec);
                                }
                            }
                        })
                        .catch(err => console.debug("Offline status check failed:", err));
                }
            }, 100); // Poll faster (100ms) for smoother progress bar animation!
        }
    </script>
</body>
</html>)rawliteral";

#endif

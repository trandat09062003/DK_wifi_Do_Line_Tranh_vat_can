#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

// --- index.html ---
const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 RC Car - Wifi Control Dashboard</title>
  <link rel="stylesheet" href="style.css">
  <!-- Google Fonts -->
  <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&display=swap" rel="stylesheet">
</head>
<body>
  <div class="glass-container">
    <!-- Header -->
    <header class="app-header">
      <div class="logo">
        <span class="pulse-dot"></span>
        <h1>MANUAL RC CONTROL</h1>
      </div>
      <div class="status-badge" id="wifi-status">Đang kết nối...</div>
    </header>

    <!-- Grid Layout -->
    <div class="dashboard-grid">
      <!-- Left Panel: Live Telemetry -->
      <section class="card telemetry-card">
        <h2 class="card-title">Giám Sát Cảm Biến</h2>
        
        <!-- Sensor Array Visualizer -->
        <div class="visualizer-container">
          <label>Trạng thái 8 Mắt Cảm Biến</label>
          <div class="sensor-array" id="sensor-array">
            <div class="sensor-dot" data-index="0"><span>S0</span></div>
            <div class="sensor-dot" data-index="1"><span>S1</span></div>
            <div class="sensor-dot" data-index="2"><span>S2</span></div>
            <div class="sensor-dot" data-index="3"><span>S3</span></div>
            <div class="sensor-dot" data-index="4"><span>S4</span></div>
            <div class="sensor-dot" data-index="5"><span>S5</span></div>
            <div class="sensor-dot" data-index="6"><span>S6</span></div>
            <div class="sensor-dot" data-index="7"><span>S7</span></div>
          </div>
        </div>

        <!-- Telemetry Stats -->
        <div class="stats-grid">
          <div class="stat-box">
            <span class="stat-label">Trạng Thái Xe</span>
            <span class="stat-value text-accent" id="val-mode">DỪNG</span>
          </div>
          <div class="stat-box">
            <span class="stat-label">Khoảng Cách Siêu Âm</span>
            <span class="stat-value text-warning" id="val-dist">-- cm</span>
          </div>
        </div>
      </section>

      <!-- Center Panel: D-Pad Controls -->
      <section class="card controls-card">
        <h2 class="card-title">Bộ Điều Khiển Thủ Công (D-Pad)</h2>
        
        <div class="dpad-container">
          <div class="dpad-row">
            <button class="dpad-btn btn-up" onclick="sendCommand('forward')">
              <span class="dpad-arrow">▲</span>
              <span class="dpad-label">TIẾN</span>
            </button>
          </div>
          <div class="dpad-row">
            <button class="dpad-btn btn-left" onclick="sendCommand('left')">
              <span class="dpad-arrow">◀</span>
              <span class="dpad-label">TRÁI</span>
            </button>
            <button class="dpad-btn btn-stop" onclick="sendCommand('stop')">
              <span class="dpad-arrow">■</span>
              <span class="dpad-label">DỪNG</span>
            </button>
            <button class="dpad-btn btn-right" onclick="sendCommand('right')">
              <span class="dpad-arrow">▶</span>
              <span class="dpad-label">PHẢI</span>
            </button>
          </div>
          <div class="dpad-row">
            <button class="dpad-btn btn-down" onclick="sendCommand('backward')">
              <span class="dpad-arrow">▼</span>
              <span class="dpad-label">LÙI</span>
            </button>
          </div>
        </div>
      </section>

      <!-- Right Panel: Tuning Panel -->
      <section class="card tuning-card">
        <h2 class="card-title">Cấu Hình Tốc Độ (Speed Tuning)</h2>
        
        <form id="tuning-form" onsubmit="event.preventDefault();">
          <!-- Slider Speed -->
          <div class="slider-group">
            <div class="slider-header">
              <label for="slider-speed">Tốc độ di chuyển</label>
              <span class="slider-value" id="val-speed">1600</span>
            </div>
            <input type="range" id="slider-speed" min="1000" max="4000" step="50" value="1600" oninput="updateSliderVal('speed', this.value)">
          </div>

          <button class="btn btn-save" onclick="sendTuningParams()">CẬP NHẬT TỐC ĐỘ</button>
        </form>
      </section>
    </div>

    <!-- Footer -->
    <footer class="app-footer">
      <p>&copy; 2026 ESP32 Manual WiFi RC Car.</p>
    </footer>
  </div>
  <script src="app.js"></script>
</body>
</html>
)rawliteral";

// --- style.css ---
const char DASHBOARD_CSS[] PROGMEM = R"rawliteral(
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Outfit', 'Inter', sans-serif;
  background: linear-gradient(135deg, #0f0c20 0%, #15102a 50%, #06040c 100%);
  color: #f1ecff;
  min-height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 20px;
  overflow-x: hidden;
}

.glass-container {
  width: 100%;
  max-width: 1200px;
  background: rgba(255, 255, 255, 0.03);
  border: 1px solid rgba(255, 255, 255, 0.06);
  border-radius: 24px;
  backdrop-filter: blur(20px);
  -webkit-backdrop-filter: blur(20px);
  box-shadow: 0 20px 50px rgba(0, 0, 0, 0.4);
  padding: 30px;
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.app-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-bottom: 1px solid rgba(255, 255, 255, 0.08);
  padding-bottom: 20px;
}

.logo {
  display: flex;
  align-items: center;
  gap: 12px;
}

.logo h1 {
  font-size: 1.5rem;
  font-weight: 800;
  letter-spacing: 2px;
  background: linear-gradient(90deg, #9b51e0, #3080ff);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}

.pulse-dot {
  width: 12px;
  height: 12px;
  background-color: #27ae60;
  border-radius: 50%;
  box-shadow: 0 0 10px #27ae60;
  animation: pulse 1.5s infinite;
}

@keyframes pulse {
  0% { transform: scale(0.9); box-shadow: 0 0 0 0 rgba(39, 174, 96, 0.7); }
  70% { transform: scale(1); box-shadow: 0 0 0 10px rgba(39, 174, 96, 0); }
  100% { transform: scale(0.9); box-shadow: 0 0 0 0 rgba(39, 174, 96, 0); }
}

.status-badge {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 6px 16px;
  border-radius: 30px;
  font-size: 0.85rem;
  font-weight: 600;
  letter-spacing: 1px;
}

.status-badge.connected {
  border-color: rgba(39, 174, 96, 0.4);
  color: #2ecc71;
  text-shadow: 0 0 8px rgba(46, 204, 113, 0.3);
}

.status-badge.disconnected {
  border-color: rgba(231, 76, 60, 0.4);
  color: #e74c3c;
  text-shadow: 0 0 8px rgba(230, 76, 60, 0.3);
}

.dashboard-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
  gap: 24px;
}

.card {
  background: rgba(255, 255, 255, 0.02);
  border: 1px solid rgba(255, 255, 255, 0.05);
  border-radius: 16px;
  padding: 24px;
  display: flex;
  flex-direction: column;
  gap: 20px;
  box-shadow: inset 0 0 15px rgba(255, 255, 255, 0.02);
}

.card-title {
  font-size: 1.15rem;
  font-weight: 600;
  letter-spacing: 1px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.05);
  padding-bottom: 10px;
  color: #d8cfff;
}

.visualizer-container {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.visualizer-container label {
  font-size: 0.85rem;
  text-transform: uppercase;
  letter-spacing: 1px;
  color: #8b80b6;
}

.sensor-array {
  display: flex;
  justify-content: space-between;
  background: rgba(0, 0, 0, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.04);
  padding: 16px 12px;
  border-radius: 12px;
}

.sensor-dot {
  width: 24px;
  height: 24px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 50%;
  display: flex;
  justify-content: center;
  align-items: center;
  font-size: 0.65rem;
  font-weight: 600;
  color: rgba(255, 255, 255, 0.4);
  transition: all 0.2s ease;
}

.sensor-dot.active {
  background: #3080ff;
  color: #fff;
  box-shadow: 0 0 12px #3080ff;
}

.stats-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}

.stat-box {
  background: rgba(0, 0, 0, 0.15);
  border: 1px solid rgba(255, 255, 255, 0.03);
  border-radius: 12px;
  padding: 12px 16px;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.stat-label {
  font-size: 0.75rem;
  color: #8b80b6;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.stat-value {
  font-size: 1.15rem;
  font-weight: 600;
  color: #f1ecff;
}

.text-accent {
  color: #3080ff;
}

.text-warning {
  color: #f2994a;
}

/* D-PAD Layout */
.dpad-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
  margin: 10px 0;
}

.dpad-row {
  display: flex;
  justify-content: center;
  gap: 10px;
}

.dpad-btn {
  width: 90px;
  height: 65px;
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 14px;
  background: rgba(255, 255, 255, 0.04);
  color: #f1ecff;
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  outline: none;
}

.dpad-btn:hover {
  background: rgba(255, 255, 255, 0.12);
  border-color: rgba(255, 255, 255, 0.25);
  transform: translateY(-2px);
}

.dpad-btn:active {
  transform: translateY(1px);
  background: rgba(255, 255, 255, 0.2);
}

.dpad-arrow {
  font-size: 1.3rem;
  line-height: 1;
}

.dpad-label {
  font-size: 0.65rem;
  font-weight: 600;
  letter-spacing: 1px;
  margin-top: 4px;
  color: rgba(255, 255, 255, 0.6);
}

.dpad-btn.btn-stop {
  background: linear-gradient(135deg, #eb5757 0%, #c02b2b 100%);
  border: none;
  box-shadow: 0 4px 15px rgba(235, 87, 87, 0.3);
}

.dpad-btn.btn-stop .dpad-label {
  color: #fff;
}

.dpad-btn.btn-stop:hover {
  box-shadow: 0 6px 20px rgba(235, 87, 87, 0.4);
  transform: translateY(-2px);
}

.dpad-btn.btn-stop:active {
  transform: translateY(1px);
}

.slider-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
  margin-bottom: 18px;
}

.slider-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 0.85rem;
  color: #8b80b6;
}

.slider-value {
  background: rgba(255, 255, 255, 0.05);
  padding: 2px 8px;
  border-radius: 6px;
  font-weight: 600;
  color: #f1ecff;
}

input[type="range"] {
  -webkit-appearance: none;
  appearance: none;
  background: rgba(255, 255, 255, 0.08);
  height: 6px;
  border-radius: 3px;
  outline: none;
}

input[type="range"]::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: #3080ff;
  cursor: pointer;
  box-shadow: 0 0 10px rgba(48, 128, 255, 0.8);
  transition: transform 0.1s ease;
}

input[type="range"]::-webkit-slider-thumb:hover {
  transform: scale(1.2);
}

.btn-save {
  width: 100%;
  border: none;
  border-radius: 12px;
  padding: 14px 20px;
  font-family: inherit;
  font-size: 0.95rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  letter-spacing: 1px;
  background: linear-gradient(90deg, #9b51e0, #802fcf);
  color: white;
  box-shadow: 0 4px 15px rgba(155, 81, 224, 0.3);
}

.btn-save:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(155, 81, 224, 0.4);
}

.app-footer {
  border-top: 1px solid rgba(255, 255, 255, 0.08);
  padding-top: 20px;
  text-align: center;
  font-size: 0.75rem;
  color: #8b80b6;
  letter-spacing: 0.5px;
}

@media (max-width: 768px) {
  body { padding: 10px; }
  .glass-container { padding: 20px; }
  .logo h1 { font-size: 1.25rem; }
}
)rawliteral";

// --- app.js ---
const char DASHBOARD_JS[] PROGMEM = R"rawliteral(
let telemetryInterval = null;
const TELEMETRY_POLL_MS = 250;

const CMD_MAP = {
  0: "DỪNG HẲN",
  1: "TIẾN THẲNG",
  2: "LÙI THẲNG",
  3: "XOAY TRÁI",
  4: "XOAY PHẢI"
};

window.addEventListener('DOMContentLoaded', () => {
  startTelemetryPolling();
});

function updateSliderVal(param, value) {
  document.getElementById(`val-${param}`).innerText = value;
}

function startTelemetryPolling() {
  if (telemetryInterval) clearInterval(telemetryInterval);
  
  telemetryInterval = setInterval(() => {
    fetch('/api/telemetry')
      .then(response => {
        if (!response.ok) throw new Error("Mất kết nối mạng");
        return response.json();
      })
      .then(data => {
        updateTelemetryUI(data);
        setConnectionStatus(true);
      })
      .catch(error => {
        console.error("Lỗi kết nối:", error);
        setConnectionStatus(false);
      });
  }, TELEMETRY_POLL_MS);
}

function setConnectionStatus(isConnected) {
  const badge = document.getElementById('wifi-status');
  if (isConnected) {
    badge.innerText = "ĐÃ KẾT NỐI";
    badge.className = "status-badge connected";
  } else {
    badge.innerText = "MẤT KẾT NỐI";
    badge.className = "status-badge disconnected";
  }
}

function updateTelemetryUI(data) {
  // Cảm biến
  const sensorVal = data.sensor;
  const dots = document.querySelectorAll('.sensor-dot');
  dots.forEach(dot => {
    const idx = parseInt(dot.getAttribute('data-index'));
    const bitShift = 7 - idx;
    const isActive = (sensorVal >> bitShift) & 0x01;
    if (isActive) {
      dot.classList.add('active');
    } else {
      dot.classList.remove('active');
    }
  });

  // Trạng thái xe
  const cmdVal = data.cmd;
  document.getElementById('val-mode').innerText = CMD_MAP[cmdVal] || `Trạng thái ${cmdVal}`;

  // Khoảng cách siêu âm
  const dist = data.dist;
  if (dist >= 999) {
    document.getElementById('val-dist').innerText = "Trống trải (>1m)";
  } else {
    document.getElementById('val-dist').innerText = `${dist} cm`;
  }

  // Tốc độ
  if (!document.activeElement || document.activeElement.id !== 'slider-speed') {
    updateSliderInput('speed', data.speed);
  }
}

function updateSliderInput(param, value) {
  const slider = document.getElementById(`slider-${param}`);
  const displayVal = document.getElementById(`val-${param}`);
  if (displayVal && slider && document.activeElement !== slider) {
    slider.value = value;
    displayVal.innerText = value;
  }
}

function sendCommand(action) {
  console.log(`Sending command: ${action}`);
  fetch('/api/control', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
    },
    body: `action=${action}`
  })
  .then(response => {
    if (!response.ok) throw new Error("Gửi lệnh thất bại");
    return response.text();
  })
  .catch(err => {
    console.error(err);
  });
}

function sendTuningParams() {
  const speed = document.getElementById('slider-speed').value;
  console.log(`Sending speed parameter: Speed=${speed}`);
  
  fetch('/api/tune', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
    },
    body: `speed=${speed}`
  })
  .then(response => {
    if (!response.ok) throw new Error("Cập nhật tốc độ thất bại");
    alert("Cập nhật tốc độ di chuyển thành công!");
  })
  .catch(err => {
    alert("Lỗi khi gửi tham số tốc độ!");
    console.error(err);
  });
}
)rawliteral";

#endif

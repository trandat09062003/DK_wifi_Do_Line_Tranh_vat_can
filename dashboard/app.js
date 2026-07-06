// Real-time update loop
let telemetryInterval = null;
const TELEMETRY_POLL_MS = 250;

// Mapping of ESP32 state values (g_trangThai) to readable text
const STATE_MAP = {
  9: "Đang chờ phím nhấn",
  10: "Bắt đầu khởi động (State 10)",
  11: "Đang chạy thẳng (State 11)",
  12: "Đang chạy ôm cua (State 12)",
  13: "Sắp tới vạch đích (State 13)",
  14: "Giảm tốc về đích (State 14)",
  15: "Chạy nhanh liên tục (State 15)",
  91: "Đang chạy thử nghiệm PID",
  100: "Đã dừng hoàn toàn (State 100)"
};

// Initial state load
window.addEventListener('DOMContentLoaded', () => {
  // Start polling telemetry
  startTelemetryPolling();
});

// Update display value when slider is moved
function updateSliderVal(param, value) {
  document.getElementById(`val-${param}`).innerText = value;
}

// Start polling API
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

// Update connection status badge
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

// Update UI elements based on JSON telemetry data
function updateTelemetryUI(data) {
  // 1. Update 8 sensor dots based on binary sensor value
  const sensorVal = data.sensor; // e.g. 60 (decimal) -> 0b00111100
  const dots = document.querySelectorAll('.sensor-dot');
  
  dots.forEach(dot => {
    const idx = parseInt(dot.getAttribute('data-index'));
    // Sensor bit index matches: S0 is MSB (bit 7), S7 is LSB (bit 0)
    const bitShift = 7 - idx;
    const isActive = (sensorVal >> bitShift) & 0x01;
    
    if (isActive) {
      dot.classList.add('active');
    } else {
      dot.classList.remove('active');
    }
  });

  // 2. Update stat boxes
  // Speed mode
  let modeText = "N/A";
  if (data.mode === 2) modeText = "Chế độ 2 (Chậm)";
  else if (data.mode === 3) modeText = "Chế độ 3 (Nhanh)";
  else if (data.mode === 1) modeText = "Chế độ 1 (Test)";
  document.getElementById('val-mode').innerText = modeText;

  // Running State
  const stateVal = data.state;
  document.getElementById('val-state').innerText = STATE_MAP[stateVal] || `Trạng thái ${stateVal}`;

  // Ultrasonic distance
  const dist = data.dist;
  if (dist >= 999) {
    document.getElementById('val-dist').innerText = "Trống trải (>1m)";
  } else {
    document.getElementById('val-dist').innerText = `${dist} cm`;
  }

  // PID Error
  document.getElementById('val-pid-err').innerText = data.pid_err;

  // 3. Pre-fill sliders on first load if they aren't currently being dragged
  if (!document.activeElement || !document.activeElement.id.startsWith('slider-')) {
    updateSliderInput('kp', data.kp);
    updateSliderInput('kd', data.kd);
    updateSliderInput('speed2', data.speed2);
    updateSliderInput('speed3', data.speed3);
  }
}

// Safely update slider input values without locking user dragging
function updateSliderInput(param, value) {
  const slider = document.getElementById(`slider-${param}`);
  const displayVal = document.getElementById(`val-${param}`);
  if (slider && displayVal && document.activeElement !== slider) {
    slider.value = value;
    displayVal.innerText = value;
  }
}

// Send simple control command via POST
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
  .then(resText => {
    console.log("Kết quả phản hồi:", resText);
  })
  .catch(err => {
    alert("Không thể gửi lệnh điều khiển. Vui lòng kiểm tra kết nối Wi-Fi!");
    console.error(err);
  });
}

// Send tuned parameters (Kp, Kd, Speed2, Speed3)
function sendTuningParams() {
  const kp = document.getElementById('slider-kp').value;
  const kd = document.getElementById('slider-kd').value;
  const speed2 = document.getElementById('slider-speed2').value;
  const speed3 = document.getElementById('slider-speed3').value;

  console.log(`Sending tuning parameters: Kp=${kp}, Kd=${kd}, Speed2=${speed2}, Speed3=${speed3}`);
  
  fetch('/api/tune', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
    },
    body: `kp=${kp}&kd=${kd}&speed2=${speed2}&speed3=${speed3}`
  })
  .then(response => {
    if (!response.ok) throw new Error("Gửi tham số cấu hình thất bại");
    return response.text();
  })
  .then(resText => {
    alert("Cập nhật tham số thành công!");
    console.log("Kết quả phản hồi:", resText);
  })
  .catch(err => {
    alert("Lỗi khi gửi tham số cấu hình!");
    console.error(err);
  });
}

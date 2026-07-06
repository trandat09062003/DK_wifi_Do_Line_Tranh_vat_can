# Dự án Xe Robot Tự Hành ESP32 (Dò line, Tránh vật cản & Điều khiển qua Wifi)

Dự án phát triển hệ thống điều khiển cho xe robot 2 bánh tự hành sử dụng chip ESP32, mạch cầu H L298N, cụm 8 mắt đọc hồng ngoại phản xạ, cảm biến siêu âm HC-SR04 gắn trên Servo SG90.

---

## 1. Sơ đồ nối dây (Pin Mapping)

### ESP32 Dev Module (38 chân)

| Tên Thiết Bị / Chức năng | Chân Linh Kiện | Chân GPIO trên ESP32 |
| :--- | :--- | :--- |
| **Còi báo (Buzzer)** | Chân dương (+) | **GPIO 12** |
| **Nút nhấn SW2** | Chân tín hiệu | **GPIO 22** (PULLUP) |
| **Động cơ Trái (Left Motor)** | IN1 / IN2 / ENA | **GPIO 18 / GPIO 19 / GPIO 27** (PWM) |
| **Động cơ Phải (Right Motor)**| IN3 / IN4 / ENB | **GPIO 16 / GPIO 17 / GPIO 14** (PWM) |
| **Servo SG90** | Dây tín hiệu cam | **GPIO 13** (PWM) |
| **Cảm biến Siêu âm** | TRIG / ECHO | **GPIO 26 / GPIO 25** |
| **Cụm 8 mắt cảm biến Line** | S0, S1, S2, S3, S4, S5, S6, S7 | **GPIO 32, 33, 34, 35, 36, 39, 21, 4** |

---

## 2. Cấu trúc thư mục mã nguồn

Mã nguồn được phân tách thành các Task chạy độc lập trong thư mục `main/`:

*   **`main/ServoTest/`**: File test quay quét Servo SG90 theo các góc tròn 30 - 60 - 90 - 120 độ sau mỗi 2 giây.
*   **`main/Task1_WifiControl/`**: Chế độ điều khiển xe thủ công (RC Car) qua giao diện Web Dashboard D-Pad kết nối qua mạng Wifi `VIETSET_TECH` (hoặc tự phát AP dự phòng). Truy cập qua địa chỉ: `http://robot.local` hoặc IP động do router cấp.
*   **`main/Task2_PD_LineFollower/`**: Chế độ tự hành bám vạch đen dùng thuật toán PD (Proportional-Derivative) cải tiến, sử dụng cấu hình so sánh tĩnh (Static Calibration) giúp bỏ qua bước học màu phức tạp. Ấn phím **SW2** để Start/Stop.
*   **`main/Task3_ObstacleAvoidance/`**: Chế độ tự do di chuyển tránh vật cản (Wander & Avoid) không bám vạch. Gặp cản dưới 20cm xe sẽ tự động phanh, lùi lại 800ms ở lực -1500, sau đó dùng Servo quét siêu âm tìm hướng thoáng nhất để rẽ đi tiếp. Ấn phím **SW2** để Start/Stop.
*   **`main/HardwareTest/`**: Code kiểm thử phần cứng thô ban đầu.

---

## 3. Cách nạp và chạy chương trình

1.  Cài đặt board ESP32 trên Arduino IDE.
2.  Mở thư mục con tương ứng với chế độ muốn chạy (ví dụ `main/Task2_PD_LineFollower/Task2_PD_LineFollower.ino`).
3.  Kết nối ESP32 với máy tính qua cáp USB, chọn đúng Board và Cổng COM trong Arduino IDE.
4.  Nhấn **Upload** để nạp code.
5.  Đặt xe vào vạch/sân chạy và nhấn **SW2 (GPIO 22)** để bắt đầu vận hành. Nhấn lại **SW2** để dừng khẩn cấp.

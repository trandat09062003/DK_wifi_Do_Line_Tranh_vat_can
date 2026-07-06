# Dự án Xe Robot Tự Hành ESP32 (Dò line, Tránh vật cản & Điều khiển qua Wifi)

Dự án phát triển hệ thống điều khiển cho xe robot 2 bánh tự hành sử dụng chip ESP32 Dev Module, mạch cầu H L298N, cụm 8 mắt đọc hồng ngoại phản xạ, cảm biến siêu âm HC-SR04 gắn trên Servo SG90.

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
| **Cụm 8 mắt cảm biến Line** | S0 -> S7 (Từ trái sang phải) | **GPIO 32, 33, 34, 35, 36, 39, 21, 4** |

*Lưu ý: Luôn nối chung dây GND của Driver L298N và ESP32.*

---

## 2. Các chức năng chính (Chia thành các thư mục trong main/)

*   **`main/ServoTest/`**: Chương trình test quay quét Servo SG90 tuần tự qua các góc tròn `30° -> 60° -> 90° -> 120°` sau mỗi 2 giây để cân chỉnh cơ khí của giá đỡ cảm biến.
*   **`main/Task1_WifiControl/`**: Chế độ lái xe thủ công qua giao diện Web Dashboard D-Pad. Tích hợp tính năng Turbo Turn (tự động tăng công suất bánh khi rẽ) giúp vượt ma sát tĩnh để xoay xe mượt mà.
*   **`main/Task2_PD_LineFollower/`**: Chế độ tự hành bám làn dùng thuật toán PD (Proportional-Derivative) cải tiến. Sử dụng cấu hình so sánh tĩnh (Static Calibration) giúp bỏ qua bước học màu phức tạp. Nhấn nút **SW2** để Start/Stop.
*   **`main/Task3_ObstacleAvoidance/`**: Chế độ tự do di chuyển tránh vật cản (Wander & Avoid) không bám vạch. Gặp cản dưới 20cm xe sẽ tự động phanh, lùi lại 800ms ở lực -1500, sau đó dùng Servo quét siêu âm tìm hướng thoáng nhất để rẽ đi tiếp. Ấn phím **SW2** để Start/Stop.
*   **`main/HardwareTest/`**: Code kiểm thử phần cứng thô ban đầu.

---

## 3. Hướng dẫn kết nối Web Dashboard điều khiển (Task 1)

Để mở trang web điều khiển động cơ trên điện thoại hoặc máy tính, bạn làm theo các bước sau:

### Bước 1: Điền thông tin Wifi của bạn vào code
Mở file [Config.h](file:///c:/Users/DELL/OneDrive%20-%20Hanoi%20University%20of%20Science%20and%20Technology/Desktop/Xe_Dò_Line/main/Task1_WifiControl/Config.h) trong Task 1, sửa lại SSID và Mật khẩu Wifi của bạn:
```cpp
#define WIFI_SSID "TÊN_WIFI"
#define WIFI_PASS "MẬT_KHẨU"
```

### Bước 2: Nạp code và lấy IP động (DHCP) để truy cập
Mặc định dự án được cấu hình chạy ở chế độ **cấp IP động (DHCP)** để tránh lỗi trùng lặp/xung đột địa chỉ IP trong mạng nội bộ.

Sau khi bật nguồn xe:
1.  **Cách truy cập nhanh:** Mở trình duyệt web gõ địa chỉ: **`http://robot.local`** (Hỗ trợ nhờ giao thức mDNS).
2.  **Cách truy cập qua IP:** Mở Serial Monitor trên Arduino IDE (tốc độ baud `115200`), xem IP do Router cấp hiển thị ở màn hình log (ví dụ: `http://192.168.1.50`). Gõ địa chỉ IP đó vào thanh địa chỉ trình duyệt của điện thoại/máy tính đang bắt chung mạng Wifi.

### Bước 3: Chế độ phát AP dự phòng (Khi đi test sân không có Wifi)
Nếu xe không thể kết nối tới mạng Wifi nhà bạn sau 8 giây, nó sẽ tự động phát ra một mạng Wifi riêng:
*   **Tên Wifi (SSID):** `ESP32_Robot_Line`
*   **Mật khẩu:** `12345678`
*   **Địa chỉ truy cập:** Kết nối điện thoại vào Wifi này và mở trình duyệt gõ: **`http://192.168.4.1`** để điều khiển xe ngay lập tức.

---

## 4. Hướng dẫn nạp chương trình

1.  Dùng Arduino IDE mở tệp `.ino` tương ứng của từng Task (Ví dụ: `Task2_PD_LineFollower.ino`).
2.  Chọn board là **ESP32 Dev Module**, chọn đúng cổng COM.
3.  Cấu hình nạp: Chọn board ESP32 Dev Module, tần số Flash 80MHz, chế độ QIO, và cổng COM tương ứng của cáp nạp.
4.  Nhấn nút nạp (Upload). Mở Serial Monitor chọn baud **115200** để xem log hoạt động.

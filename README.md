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
*   **`main/Task2_PD_LineFollower/`**: Chế độ tự hành bám làn dùng thuật toán PD (Proportional-Derivative) cải tiến. 
    *   **Nguyên lý hoạt động:** Xe liên tục đọc giá trị Analog từ 8 mắt cảm biến hồng ngoại để tính toán vị trí tương đối của xe so với vạch đen (sai số lệch tâm `g_posPID` dao động từ `-10500` đến `+10500`).
    *   **Bộ lọc nhiễu & Tăng ích phi tuyến (Variable Gain):** Sai số được lọc qua một bộ lọc thông thấp (Low-pass filter) để giảm hiện tượng giật lắc đầu xe. Ngoài ra, hệ thống tự động nhân thêm hệ số phi tuyến (Gain tăng khi lệch nhiều) giúp xe ôm các cua gắt cực nhạy nhưng vẫn đi thẳng rất đằm trên đoạn thẳng.
    *   **Xử lý ngã tư:** Khi phát hiện đồng thời cả các mắt rìa trái (S0/S1) và rìa phải (S6/S7) cùng chạm vạch đen, xe nhận dạng đó là ngã tư và tự động khóa sai số PID về `0` để đi thẳng băng qua giao lộ mà không bị nhiễu vạch cắt ngang làm lệch hướng.
    *   **Cấu hình cảm biến tĩnh (Static Calibration):** Hệ thống gán cứng các ngưỡng đo thực tế (`Black = 700`, `White = 3200`, `Threshold = 1950`) giúp xe nhận diện vạch đen ngay lập tức khi bật nguồn, bỏ qua bước học màu (calib) quét tay phức tạp.
    *   **Start/Stop một chạm:** Nhấn nút **SW2 (GPIO 22)** để bắt đầu chạy bám làn ở tốc độ mặc định `1600` (định nghĩa qua `NUT2_SPEED` trong `Config.h`). Nhấn lại nút **SW2** khi xe đang chạy để dừng xe khẩn cấp.
*   **`main/Task3_ObstacleAvoidance/`**: Chế độ tự do di chuyển tránh vật cản (Wander & Avoid) không bám vạch.
    *   **Nguyên lý quét radar tránh cản:**
        1.  Cảm biến siêu âm HC-SR04 liên tục đo khoảng cách phía trước mỗi 80ms. Nếu phát hiện cản cách xe dưới **`20cm`** (duy trì ổn định qua 2 lần đo liên tiếp để chống nhiễu), xe lập tức dừng và bíp còi báo hiệu.
        2.  Xe kích hoạt động cơ lùi mạnh mẽ ở lực **`-1500` trong `800ms`** để lùi xe ra sau một khoảng an toàn, tạo góc nhìn rộng hơn cho radar siêu âm và tránh va chạm hông xe khi bẻ lái.
        3.  Động cơ Servo SG90 quay cụm siêu âm sang Trái (45°) dừng 600ms đo khoảng cách, sau đó quay sang Phải (135°) dừng 600ms đo khoảng cách, rồi trả về góc giữa (90°).
        4.  Xe so sánh khoảng cách Trái/Phải để chọn lối thoát:
            *   Nếu cả hai hướng đều bị chặn dưới 25cm: Xe tự động xoay tại chỗ 180 độ để quay đầu đi ngược lại.
            *   Nếu bên nào thoáng hơn: Xe tự xoay hướng bánh về bên đó trong 550ms, dừng xe, rồi tiếp tục tiến thẳng tìm đường đi mới.
    *   **Start/Stop:** Nhấn nút **SW2 (GPIO 22)** để bắt đầu di chuyển tự do hoặc dừng xe khẩn cấp tương tự Task 2.
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

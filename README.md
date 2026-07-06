# Dự án Xe Robot Tự Hành ESP32: Dò Line PID, Tránh Vật Cản & Điều Khiển Wi-Fi

Dự án này chứa mã nguồn toàn diện cho xe robot tự hành 2 bánh dựa trên vi điều khiển ESP32 Dev Module. Hệ thống tích hợp các chức năng nâng cao bao gồm điều khiển thủ công qua Web Dashboard bằng Wi-Fi, tự động bám làn (dò line) sử dụng thuật toán PD (Proportional-Derivative) cải tiến, và tự động tránh vật cản bằng cảm biến siêu âm HC-SR04 kết hợp Servo quét góc.

---

## 1. Sơ Đồ Đấu Nối Phần Cứng (Pinout Configuration)

Dưới đây là sơ đồ kết nối dây giữa board điều khiển ESP32 Dev Module (loại 38 chân) với các module ngoại vi trên xe:

### Động Cơ & Mạch Cầu H L298N
Mạch cầu H L298N nhận tín hiệu số để đảo chiều và xung PWM từ ESP32 để điều khiển tốc độ:
*   **Motor Trái (Left Motor):**
    *   `LIN1` (Chiều quay 1) $\rightarrow$ **GPIO 18**
    *   `LIN2` (Chiều quay 2) $\rightarrow$ **GPIO 19**
    *   `LENA` (Chân PWM tốc độ) $\rightarrow$ **GPIO 27**
*   **Motor Phải (Right Motor):**
    *   `RIN1` (Chiều quay 1) $\rightarrow$ **GPIO 16**
    *   `RIN2` (Chiều quay 2) $\rightarrow$ **GPIO 17**
    *   `RENA` (Chân PWM tốc độ) $\rightarrow$ **GPIO 14**

### Cảm Biến Khoảng Cách & Servo Quét
*   **Cảm biến Siêu âm HC-SR04:**
    *   `TRIG` (Kích hoạt phát sóng) $\rightarrow$ **GPIO 26**
    *   `ECHO` (Đo thời gian sóng phản hồi) $\rightarrow$ **GPIO 25**
*   **Động cơ Servo SG90 (Quay cảm biến):**
    *   `PWM` (Tín hiệu góc quay) $\rightarrow$ **GPIO 13**

### Khối Cảm Biến Dò Line (8 Mắt Hồng Ngoại TCRT5000)
Đọc giá trị Analog từ 8 mắt để xử lý giải thuật nội suy vị trí:
*   `S0` (Mắt ngoài cùng bên trái) $\rightarrow$ **GPIO 32**
*   `S1` $\rightarrow$ **GPIO 33**
*   `S2` $\rightarrow$ **GPIO 34**
*   `S3` $\rightarrow$ **GPIO 35**
*   `S4` $\rightarrow$ **GPIO 36**
*   `S5` $\rightarrow$ **GPIO 39**
*   `S6` $\rightarrow$ **GPIO 21**
*   `S7` (Mắt ngoài cùng bên phải) $\rightarrow$ **GPIO 4**

### Thiết Bị Phụ Trợ
*   **Còi báo (Buzzer):** Chân tín hiệu tích cực mức cao $\rightarrow$ **GPIO 12**
*   **Phím bấm SW2 (Start/Stop):** Nhấn phím để kích hoạt chế độ tự hành hoặc dừng khẩn cấp $\rightarrow$ **GPIO 22** (sử dụng điện trở kéo lên nội bộ `INPUT_PULLUP`).

---

## 2. Chi Tiết Kỹ Thuật Các Chế Độ Vận Hành

Mã nguồn được tổ chức thành 3 chế độ (Task) độc lập và một chương trình test phụ:

### Task 1: Điều Khiển Qua Wifi (WifiControl)
*   **Cách hoạt động:** ESP32 khởi tạo Web Server trên cổng 80 và kết nối vào mạng Wi-Fi cấu hình sẵn trong `Config.h` (mặc định là `VIETSET_TECH` / pass: `vs68686868`). Nếu kết nối thất bại sau 8 giây, ESP32 tự động chuyển sang chế độ phát Access Point (AP) dự phòng tên là `ESP32_Robot_Line` (mật khẩu: `12345678`, IP: `192.168.4.1`).
*   **Tên miền mDNS:** Người dùng có thể truy cập trang điều khiển thông qua địa chỉ ảo **`http://robot.local`** mà không cần tra cứu địa chỉ IP.
*   **Giao diện:** Thiết kế Glassmorphism tối giản hiển thị dữ liệu thời gian thực (telemetry) gồm khoảng cách vật cản, trạng thái 8 mắt dò line dưới dạng đèn LED ảo, và cụm nút D-Pad để tiến, lùi, rẽ hướng, tăng/giảm tốc độ.
*   **Mô-men xoắn xoay cua (Turbo Turn):** Khi nhận lệnh xoay Trái/Phải từ web, hệ thống tự động cộng thêm `600` đơn vị PWM vào tốc độ hiện tại để đảm bảo xe thắng được ma sát tĩnh của lốp cao su trên sàn nhà.

### Task 2: Dò Line Thuật Toán PD (PD_LineFollower)
*   **Giải thuật bám vạch:** Sử dụng thuật toán PD (Proportional - Derivative) cải tiến. Hệ thống tính toán sai số lệch tâm (Error) từ 0 đến 8 mắt và truyền qua bộ lọc thông thấp (Low-pass filter) tần số cắt thấp để triệt tiêu nhiễu rung đầu xe.
*   **Tăng ích phi tuyến (Variable Gain):** Hệ số nhân sai số sẽ tự động tăng tỉ lệ thuận với độ lệch của xe so với vạch. Xe lệch càng nhiều thì lực lái chỉnh góc cua càng mạnh.
*   **Xử lý ngã tư:** Khi các cảm biến rìa phát hiện vạch ngang đen cùng lúc, giải thuật tự động khóa sai số ở mức `0` để ép xe đi thẳng băng qua giao lộ mà không bị bẻ lái lệch hướng.
*   **Không cần Calib:** Sử dụng cấu hình cảm biến tĩnh (Static Calibration) với các ngưỡng thiết lập sẵn (`Black = 700`, `White = 3200`, `Threshold = 1950`) giúp loại bỏ hoàn toàn thao tác học màu phức tạp khi khởi động.
*   **Cơ chế một phím bấm:** Nhấn nút **SW2** để bắt đầu chạy bám làn ở tốc độ mặc định `1600` (được định nghĩa qua `NUT2_SPEED` trong `Config.h`). Nhấn lại nút **SW2** khi xe đang chạy để dừng xe ngay lập tức.

### Task 3: Tự Động Tránh Vật Cản Tự Do (ObstacleAvoidance)
*   **Hành vi:** Xe chạy tự do tiến thẳng phía trước và giám sát radar siêu âm liên tục. Không bám vạch.
*   **Kịch bản tránh vật cản nâng cao:**
    1.  Khi khoảng cách phía trước `< 20cm` (được lọc độ ổn định qua 2 lần đo liên tiếp), xe lập tức phanh dừng hẳn và phát âm còi báo.
    2.  Xe kích hoạt động cơ lùi với lực khỏe **`-1500` trong `800ms`** để lùi xa vật cản, tạo góc quét rộng cho Servo rẽ lái an toàn, tránh va chạm hông xe.
    3.  Động cơ Servo SG90 quay cụm siêu âm sang Trái (45°) và Phải (135°) để đo khoảng trống hai bên hông.
    4.  So sánh khoảng cách: Xe rẽ về hướng thoáng hơn (bên có khoảng cách đo được lớn hơn) bằng cách quay tại chỗ trong `550ms`, sau đó tiếp tục đi thẳng. Nếu cả hai hướng đều bị chặn dưới `25cm`, xe tự xoay ngược đầu 180° để quay lại.
*   **Start/Stop:** Sử dụng nút bấm **SW2** để bật/tắt hành trình tương tự như Task 2.

### HardwareTest & ServoTest
*   **`main/HardwareTest/`**: Chương trình test cơ bản để đo hoạt động ban đầu của động cơ và đọc giá trị cảm biến thô.
*   **`main/ServoTest/`**: Code quét góc Servo liên tục qua các vị trí góc tròn `30° -> 60° -> 90° -> 120°` sau mỗi 2 giây để kiểm tra và căn chỉnh cơ khí của giá đỡ siêu âm.

---

## 4. Hướng Dẫn Nạp Chương Trình & Chạy Thử

1.  **Cài đặt Thư viện & Board trên Arduino IDE:**
    *   Cài đặt gói board ESP32 từ Espressif (khuyến nghị phiên bản 3.x trở lên).
    *   Cấu hình thông số nạp: Chọn board **ESP32 Dev Module**, tần số Flash **80MHz**, chế độ **QIO**, và cổng COM tương ứng của cáp nạp.
2.  **Cách nạp và chạy:**
    *   Mở tệp `.ino` bên trong thư mục Task cần kiểm tra (ví dụ: `main/Task2_PD_LineFollower/Task2_PD_LineFollower.ino`).
    *   Biên dịch và nhấn nạp chương trình.
    *   Mở **Serial Monitor** ở tốc độ baud **115200** để xem thông tin chẩn đoán thời gian thực.
    *   Bấm nút **SW2** trên xe để bắt đầu chạy thử nghiệm.

---

## 5. Hướng Dẫn Cấu Hình Wi-Fi & Khắc Phục Xung Đột IP

Khi chạy **Task 1 (Wifi Control)**, để thiết bị của bạn (điện thoại/máy tính) có thể kết nối được tới trang Web Dashboard điều khiển xe, bạn cần đảm bảo ESP32 và máy tính nằm trên cùng một lớp mạng (subnet). 

### Bước 1: Xem lớp mạng của máy tính (Windows)
1.  Nhấn tổ hợp phím `Windows + R`, gõ `cmd` rồi nhấn Enter để mở Terminal.
2.  Nhập lệnh sau và nhấn Enter:
    ```bash
    ipconfig
    ```
3.  Tìm dòng **IPv4 Address** và **Default Gateway** của card mạng Wi-Fi bạn đang kết nối. Ví dụ:
    *   *IPv4 Address:* `192.168.1.15` (Nghĩa là máy tính của bạn thuộc lớp mạng `192.168.1.x`).
    *   *Default Gateway:* `192.168.1.1` (Đây là địa chỉ IP của Router mạng).

### Bước 2: Cấu hình lại Wi-Fi trong code
Mở tệp [Config.h](file:///c:/Users/DELL/OneDrive%20-%20Hanoi%20University%20of%20Science%20and%20Technology/Desktop/Xe_Dò_Line/main/Task1_WifiControl/Config.h) của **Task 1** và chỉnh sửa thông tin mạng Wi-Fi nhà bạn:
```cpp
#define WIFI_SSID "TÊN_WIFI_NHÀ_BẠN"
#define WIFI_PASS "MẬT_KHẨU_WIFI_NHÀ_BẠN"
```

### Bước 3: Lựa chọn chế độ cấp IP cho Dashboard
Bạn có 2 lựa chọn để truy cập vào xe:

#### Lựa chọn A: Sử dụng IP Động (Khuyến nghị - DHCP)
*   **Cách làm:** Hãy đảm bảo dòng `#define WIFI_USE_STATIC_IP` đang được comment lại (như code mặc định):
    ```cpp
    // #define WIFI_USE_STATIC_IP
    ```
*   **Cách truy cập:** Khi bật nguồn, Router sẽ tự cấp 1 IP trống cho xe. Bạn chỉ cần mở trình duyệt và gõ địa chỉ: **`http://robot.local`** (hoặc mở Serial Monitor ở tốc độ 115200 để xem IP cụ thể của xe được cấp).

#### Lựa chọn B: Sử dụng IP Tĩnh (Static IP)
*   **Cách làm:** Nếu bạn muốn gán cứng IP cố định cho xe để không cần tra cứu Serial Monitor, hãy bỏ comment dòng này:
    ```cpp
    #define WIFI_USE_STATIC_IP
    ```
*   **Cách cấu hình:** Dựa vào thông số lấy từ **Bước 1 (ipconfig)**, thay đổi các thông số tương ứng trong `Config.h`:
    *   Nếu máy tính có IP là `192.168.1.15` (lớp mạng `1.x`), bạn gán:
        ```cpp
        #define WIFI_STATIC_IP_ADDR  192, 168, 1, 150  // Chọn 150 để tránh trùng thiết bị khác
        #define WIFI_GATEWAY_ADDR    192, 168, 1, 1    // Trùng với Default Gateway
        #define WIFI_SUBNET_MASK     255, 255, 255, 0
        ```
    *   Nếu máy tính có IP là `192.168.0.10` (lớp mạng `0.x`), bạn PHẢI sửa lại:
        ```cpp
        #define WIFI_STATIC_IP_ADDR  192, 168, 0, 150  // Đổi số thứ 3 thành 0
        #define WIFI_GATEWAY_ADDR    192, 168, 0, 1    // Đổi số thứ 3 thành 0
        #define WIFI_SUBNET_MASK     255, 255, 255, 0
        ```
*   **Cách truy cập:** Mở trình duyệt và gõ trực tiếp địa chỉ: **`http://192.168.x.150`** (với `x` là lớp mạng tương ứng).

### Bước 4: Chế độ AP Dự Phòng (Fallback AP)
Nếu mạng Wi-Fi cấu hình ở Bước 2 gặp sự cố hoặc mất mạng:
1.  ESP32 sẽ tự động phát mạng Wi-Fi riêng tên là: **`ESP32_Robot_Line`** (mật khẩu: **`12345678`**).
2.  Bạn chỉ cần lấy điện thoại/máy tính kết nối vào mạng Wi-Fi này.
3.  Mở trình duyệt truy cập địa chỉ IP mặc định của AP: **`http://192.168.4.1`** để điều khiển xe ngay lập tức.

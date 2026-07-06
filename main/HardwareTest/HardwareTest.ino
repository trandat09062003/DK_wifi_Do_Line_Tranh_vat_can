#include <Arduino.h>

// ==================== CẤU HÌNH CHÂN (MỚI) ====================
// Động cơ L298N
#define RIN1 16
#define RIN2 17
#define RENA 14
#define LIN1 18
#define LIN2 19
#define LENA 27

// Nút nhấn & Còi
#define BUTTON1 15  // SW1
#define BUTTON2 22  // SW2 (Mới)
#define BUTTON3 23  // SW3 (Mới)
#define BUZZER 4

// Siêu âm & Servo
#define TRIG_PIN 5
#define ECHO_PIN 21
#define SERVO_PIN 13

// Cảm biến Line
const uint8_t sensorPins[8] = {36, 39, 34, 35, 32, 33, 25, 26};

// ==================== KHỞI TẠO HÀM ====================
void writeServo(int angle) {
  // Bản đồ góc (0-180) sang độ rộng xung 14-bit (1638-7864) ở tần số 50Hz
  int duty = map(angle, 0, 180, 1638, 7864);
  ledcWrite(SERVO_PIN, duty);
}

void beep(int ms) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}

void setup() {
  Serial.begin(115200);
  
  // Còi & Nút bấm
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  // Động cơ
  pinMode(LIN1, OUTPUT);
  pinMode(LIN2, OUTPUT);
  pinMode(RIN1, OUTPUT);
  pinMode(RIN2, OUTPUT);
  digitalWrite(LIN1, LOW);
  digitalWrite(LIN2, LOW);
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, LOW);
  
  // Khởi tạo PWM Động cơ (Core v3.x)
  ledcAttach(LENA, 2000, 12); // Tần số 2kHz, Độ phân giải 12-bit (0-4095)
  ledcAttach(RENA, 2000, 12);

  // Cảm biến siêu âm
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Khởi tạo Servo (Tần số 50Hz, Độ phân giải 14-bit)
  ledcAttach(SERVO_PIN, 50, 14);

  // Bíp chào mừng
  beep(100);
  delay(100);

  // Test quét Servo
  Serial.println("[SERVO] Dang test quet Servo...");
  writeServo(90);  // Giữa
  delay(500);
  writeServo(45);  // Trái
  delay(500);
  writeServo(135); // Phải
  delay(500);
  writeServo(90);  // Trở lại giữa
  delay(500);
  Serial.println("[SYSTEM] San sang kiem tra!");
}

void loop() {
  // 1. Đọc khoảng cách siêu âm
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance = (duration > 0) ? (duration * 0.034 / 2.0) : 999.0;

  // 2. Đọc trạng thái nút nhấn (0 = NHẤN, 1 = THẢ)
  int sw1 = digitalRead(BUTTON1);
  int sw2 = digitalRead(BUTTON2);
  int sw3 = digitalRead(BUTTON3);

  // 3. Đọc giá trị Analog của 8 cảm biến dò line
  int lineValues[8];
  for (int i = 0; i < 8; i++) {
    lineValues[i] = analogRead(sensorPins[i]);
  }

  // 4. In toàn bộ log ra Serial Monitor
  Serial.print("Dist: ");
  if (distance >= 999.0) Serial.print("MAX  ");
  else { Serial.print(distance, 1); Serial.print("cm "); }

  Serial.print("| Buttons (1/2/3): ");
  Serial.print(sw1 == LOW ? "0" : "1"); Serial.print("/");
  Serial.print(sw2 == LOW ? "0" : "1"); Serial.print("/");
  Serial.print(sw3 == LOW ? "0" : "1"); Serial.print(" ");

  Serial.print("| Sensors (S0..S7): [");
  for (int i = 0; i < 8; i++) {
    Serial.print(lineValues[i]);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("]");

  // 5. Điều khiển Motor test dựa vào nút nhấn
  // Nhấn nút nào thì motor tương ứng quay tiến với tốc độ ~35% (1500/4095)
  if (sw1 == LOW && sw2 == HIGH && sw3 == HIGH) {
    // Chỉ nhấn SW1 -> Chạy xích động cơ TRÁI
    digitalWrite(LIN1, HIGH);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, 1500);

    digitalWrite(RIN1, LOW);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, 0);
  } 
  else if (sw2 == LOW && sw1 == HIGH && sw3 == HIGH) {
    // Chỉ nhấn SW2 -> Chạy xích động cơ PHẢI
    digitalWrite(LIN1, LOW);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, 0);

    digitalWrite(RIN1, HIGH);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, 1500);
  }
  else if (sw3 == LOW) {
    // Nhấn SW3 -> Chạy cả HAI động cơ
    digitalWrite(LIN1, HIGH);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, 1500);

    digitalWrite(RIN1, HIGH);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, 1500);
  }
  else {
    // Không nhấn nút nào -> Dừng toàn bộ động cơ
    digitalWrite(LIN1, LOW);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, 0);

    digitalWrite(RIN1, LOW);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, 0);
  }

  delay(300); // Chu kỳ cập nhật log 300ms
}

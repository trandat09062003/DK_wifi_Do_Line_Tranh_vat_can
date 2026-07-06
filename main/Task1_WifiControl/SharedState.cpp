#include "SharedState.h"
#include "Config.h"

hw_timer_t* g_timer = NULL;
portMUX_TYPE g_timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile long g_lastPos = 0;
volatile unsigned char g_isCalib = 0;
int g_servoPwm = 0;
volatile int g_calPID = 0;
volatile unsigned char g_sensor = 0;

unsigned int g_sensorValue[8] = {0};
unsigned int g_sensorPID[8] = {0};
unsigned int g_blackValue[8] = {0};
unsigned int g_whiteValue[8] = {0};
unsigned int g_compareValue[8] = {0};

int g_speedRunForward = 0;
int g_isIMG = 0;
volatile long g_posPID = 0;
volatile int g_cnt = 0;
volatile int g_cnt1 = 0;
unsigned char g_pattern = 11;
unsigned char g_start = 0;
unsigned char g_trangThai = 9;
int g_rememberLine = 0;
volatile int g_pidFre = 0;

int g_distanceCm = 999;
bool g_obstacleStop = false;
bool g_obstacleSlow = false;
unsigned long g_lastUltrasonicMs = 0;
bool g_obstacleBeeped = false;
volatile bool g_sensorReadPending = false;

// Khởi tạo các thông số chỉnh động mặc định
float g_pidKp = PID_KP;
int g_pidKd = PID_KD;
int g_speedMode2 = NUT2_SPEED;
int g_speedMode3 = NUT3_SPEED;

// Cấu hình ban đầu cho điều khiển thủ công
int g_manualCommand = 0; // 0: Dừng
int g_manualSpeed = 1600; // Tốc độ di chuyển mặc định


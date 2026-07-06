#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "WifiControl.h"
#include "WebDashboard.h"
#include "SharedState.h"
#include "MotorControl.h"
#include "LineSensor.h"
#include "LineFollower.h"

static WebServer server(80);

static void handleRoot() {
  server.send(200, "text/html", DASHBOARD_HTML);
}

static void handleCSS() {
  server.send(200, "text/css", DASHBOARD_CSS);
}

static void handleJS() {
  server.send(200, "application/javascript", DASHBOARD_JS);
}

static void handleTelemetry() {
  String json = "{";
  json += "\"sensor\":" + String(g_sensor) + ",";
  json += "\"dist\":" + String(g_distanceCm) + ",";
  json += "\"speed\":" + String(g_manualSpeed) + ",";
  json += "\"cmd\":" + String(g_manualCommand);
  json += "}";
  server.send(200, "application/json", json);
}

static void handleControl() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }
  
  String action = server.arg("action");
  Serial.print("[WIFI] Control action: ");
  Serial.println(action);

  if (action == "forward") {
    g_manualCommand = 1; // Tiến
  } 
  else if (action == "backward") {
    g_manualCommand = 2; // Lùi
  } 
  else if (action == "left") {
    g_manualCommand = 3; // Rẽ trái
  } 
  else if (action == "right") {
    g_manualCommand = 4; // Rẽ phải
  } 
  else if (action == "stop") {
    g_manualCommand = 0; // Dừng
  }

  server.send(200, "text/plain", "OK");
}

static void handleTune() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String speedStr = server.arg("speed");
  if (speedStr.length() > 0) {
    g_manualSpeed = speedStr.toInt();
    Serial.print("[WIFI] Updated manual speed: ");
    Serial.println(g_manualSpeed);
  }

  server.send(200, "text/plain", "OK");
}

void wifiControlInit() {
#ifdef WIFI_USE_STATIC_IP
  // Thiết lập cấu hình IP tĩnh để không đổi địa chỉ
  IPAddress local_IP(WIFI_STATIC_IP_ADDR);
  IPAddress gateway(WIFI_GATEWAY_ADDR);
  IPAddress subnet(WIFI_SUBNET_MASK);
  IPAddress dns(WIFI_GATEWAY_ADDR); // Dùng gateway làm DNS chính

  Serial.println("[WIFI] Setting Static IP configuration...");
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("[WIFI] Static IP configuration failed! Falling back to DHCP...");
  }
#endif

  // Cố gắng kết nối vào mạng Wi-Fi của người dùng
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("[WIFI] Connecting to Wi-Fi '%s'...", WIFI_SSID);
  
  unsigned long startConnectMs = millis();
  // Chờ tối đa 8 giây để kết nối
  while (WiFi.status() != WL_CONNECTED && millis() - startConnectMs < 8000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] Connected to Wi-Fi successfully!");
    Serial.print("[WIFI] Access Web Dashboard at: http://");
    Serial.println(WiFi.localIP());
  } else {
    // Nếu thất bại -> phát AP dự phòng để người dùng kết nối trực tiếp
    Serial.println("\n[WIFI] Wi-Fi connection failed. Fallback to AP Mode...");
    WiFi.softAP("ESP32_Robot_Line", "12345678");
    Serial.print("[WIFI] Access Web Dashboard at: http://");
    Serial.println(WiFi.softAPIP());
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/app.js", HTTP_GET, handleJS);
  server.on("/api/telemetry", HTTP_GET, handleTelemetry);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/tune", HTTP_POST, handleTune);

  server.begin();
  Serial.println("[WIFI] Web Server running on port 80.");
  
  if (MDNS.begin("robot")) {
    Serial.println("[WIFI] mDNS responder started. Access via: http://robot.local");
  }
}

void wifiControlUpdate() {
  server.handleClient();
}

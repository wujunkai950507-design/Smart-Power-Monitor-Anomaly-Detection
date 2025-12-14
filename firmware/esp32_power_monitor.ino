#include <WiFi.h>
#include <HTTPClient.h>
#include <PZEM004Tv30.h>

// ===== WiFi =====
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASS = "YOUR_PASS";

// ===== Google Apps Script Web App URL =====
const char* GAS_URL = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxx/exec";

// ===== PZEM v3 UART pins (ESP32) =====
static const int PZEM_RX_PIN = 16; // ESP32 RX2  <- PZEM TX
static const int PZEM_TX_PIN = 17; // ESP32 TX2  -> PZEM RX

// ===== Upload interval =====
const int UPLOAD_MS = 5000;

// ===== Anomaly detect (simple rule) =====
const float THRESHOLD_W = 800.0;   // 功率超過視為 ALERT（你可調）

// ===== PZEM Serial + Device =====
HardwareSerial PZEMSerial(2);
PZEM004Tv30 pzem(PZEMSerial);

struct PowerData {
  float voltage;
  float current;
  float power;
  float energy;
  float frequency;
  float pf;
  String status; // NORMAL / ALERT / SENSOR_ERR
};

static bool isBad(float v) { return isnan(v) || isinf(v); }

PowerData readPowerSensor() {
  PowerData d;

  d.voltage   = pzem.voltage();
  d.current   = pzem.current();
  d.power     = pzem.power();
  d.energy    = pzem.energy();
  d.frequency = pzem.frequency();
  d.pf        = pzem.pf();

  // 有任何一項讀到 NaN，視為感測器讀取失敗
  if (isBad(d.voltage) || isBad(d.current) || isBad(d.power) || isBad(d.energy)) {
    d.status = "SENSOR_ERR";
    return d;
  }

  d.status = (d.power > THRESHOLD_W) ? "ALERT" : "NORMAL";
  return d;
}

bool postToGAS(const PowerData& d) {
  if (WiFi.status() != WL_CONNECTED) return false;

  // 送 JSON（timestamp 交給 Apps Script 寫入）
  String payload = "{";
  payload += "\"voltage\":"   + String(d.voltage, 2) + ",";
  payload += "\"current\":"   + String(d.current, 3) + ",";
  payload += "\"power\":"     + String(d.power, 1) + ",";
  payload += "\"energy\":"    + String(d.energy, 3) + ",";
  payload += "\"frequency\":" + String(d.frequency, 1) + ",";
  payload += "\"pf\":"        + String(d.pf, 2) + ",";
  payload += "\"status\":\""  + d.status + "\"";
  payload += "}";

  Serial.println("POST: " + payload);

  HTTPClient http;
  http.begin(GAS_URL);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(payload);
  String res = http.getString();

  Serial.printf("HTTP %d\n", code);
  Serial.println(res);

  http.end();
  return (code >= 200 && code < 300);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // UART2 for PZEM (PZEM v3 通常 9600 8N1)
  PZEMSerial.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
  delay(200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());
}

void loop() {
  PowerData d = readPowerSensor();

  // 序列監控視窗即時顯示（展示用很加分）
  Serial.printf("V=%.2fV  I=%.3fA  P=%.1fW  E=%.3fkWh  F=%.1fHz  PF=%.2f  [%s]\n",
                d.voltage, d.current, d.power, d.energy, d.frequency, d.pf, d.status.c_str());

  // 上傳
  postToGAS(d);

  delay(UPLOAD_MS);
}

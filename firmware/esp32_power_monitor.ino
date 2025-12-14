#include <WiFi.h>
#include <HTTPClient.h>

// ===== WiFi =====
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASS = "YOUR_PASS";

// ===== Google Apps Script Web App URL =====
const char* GAS_URL = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxx/exec";

// ===== 異常門檻（你可在書審寫：規則型異常偵測）=====
const float THRESHOLD_W = 800.0;   // 超過 800W 視為異常（示例）
const int   UPLOAD_MS   = 5000;    // 5 秒上傳一次

struct PowerData {
  float voltage;
  float current;
  float power;
  float energy;
  String status; // NORMAL / ALERT
};

PowerData readPowerSensor() {
  // TODO：改成你實際的感測器讀取（PZEM-004T v3 / INA226 / INA219）
  // 這裡先用假資料讓流程跑通
  static float e = 0;
  PowerData d;
  d.voltage = 110.0 + random(-10, 10) * 0.1;
  d.current = 1.20 + random(-10, 10) * 0.01;
  d.power   = d.voltage * d.current;  // W
  e += d.power * (UPLOAD_MS / 1000.0) / 3600.0; // Wh (簡化)
  d.energy  = e;

  d.status = (d.power > THRESHOLD_W) ? "ALERT" : "NORMAL";
  return d;
}

void setup() {
  Serial.begin(115200);
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

  // 你也可以把 timestamp 留給 Apps Script 產生
  String payload = "{";
  payload += "\"voltage\":" + String(d.voltage, 2) + ",";
  payload += "\"current\":" + String(d.current, 3) + ",";
  payload += "\"power\":"   + String(d.power, 1) + ",";
  payload += "\"energy\":"  + String(d.energy, 2) + ",";
  payload += "\"status\":\"" + d.status + "\"";
  payload += "}";

  Serial.println("POST: " + payload);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(GAS_URL);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(payload);
    String res = http.getString();

    Serial.printf("HTTP %d\n", code);
    Serial.println(res);
    http.end();
  } else {
    Serial.println("WiFi lost...");
  }

  delay(UPLOAD_MS);
}

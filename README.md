# AIoT 智慧用電監測與異常偵測系統  
AIoT Smart Power Monitor + Anomaly Detection

> 使用 ESP32 讀取電力參數（電壓 / 電流 / 功率 / 電能），上傳至 Google Sheets 做即時紀錄，
> 並加入「異常用電偵測」機制，快速發現設備故障或用電異常。

---

## 🔥 作品亮點（電機電子系加分點）
- **電力量測**：電壓(V)、電流(A)、功率(W)、電能(Wh/kWh)
- **即時資料上雲**：ESP32 → Wi-Fi → Google Apps Script → Google Sheets
- **異常偵測**：
  - 規則型：瞬間功率暴增、長時間高功率、超過平均值 N 倍
  - 可擴充 AI：移動平均 / Z-score / One-Class (server-side)
- **可視化**：用 Google Sheets 圖表（或後續接 Grafana/Thingspeak）

---

## 🧠 系統架構
1. **ESP32** 讀取電力模組資料  
2. 透過 Wi-Fi 送出 JSON 到 **Google Apps Script Web App**  
3. Apps Script 將資料寫入 **Google Sheets**  
4. Sheets 生成折線圖，並根據偵測結果顯示 **ALERT / NORMAL**

---

## 🧰 硬體材料（BOM）
- ESP32 DevKit V1（或 ESP32-S3 也可）
- 電力量測模組（二選一）：
  - ✅ **PZEM-004T v3.0**（量測 AC 電壓/電流/功率/電能，適合家電/插座）
  - 或 INA226/INA219（偏 DC 量測）
- 杜邦線、麵包板（或焊接）
- （選配）OLED 0.96" I2C 顯示

> ⚠️ 若量測市電 AC，請務必注意安全與絕緣，建議使用 PZEM 成品模組與保護盒。

---

## 🔌 接線（以 PZEM-004T v3.0 + ESP32 為例）
PZEM(TX) → ESP32 RX（例如 GPIO16）  
PZEM(RX) → ESP32 TX（例如 GPIO17）  
PZEM(GND) → ESP32 GND  

> 有些 PZEM 是 5V TTL，請確認你的模組是否需要電平轉換（3.3V/5V）。

---

## 📁 Repo 結構（建議）

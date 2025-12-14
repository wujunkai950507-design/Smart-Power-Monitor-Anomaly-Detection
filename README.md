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
├─ firmware/
│ └─ esp32_power_monitor.ino
├─ apps_script/
│ └─ Code.gs
├─ docs/
│ └─ wiring.png (可放接線圖)
└─ README.md

---

## 🚀 快速開始

### Step 1：建立 Google Sheets
新增一份試算表，建立欄位（第一列）：
- timestamp
- voltage
- current
- power
- energy
- status

---

### Step 2：部署 Apps Script Web App
1. 打開 Google Sheets → 擴充功能 → Apps Script  
2. 新增 `Code.gs`，貼上 `apps_script/Code.gs` 的內容  
3. 部署 → 新增部署作業 → 類型選 **網路應用程式**
4. 權限：
   - 執行身分：我
   - 存取權：任何人
5. 複製 Web App URL，貼到 ESP32 程式的 `GAS_URL`

---

### Step 3：燒錄 ESP32
使用 Arduino IDE / PlatformIO 上傳 `firmware/esp32_power_monitor.ino`

---

## ✅ 異常偵測邏輯（可在履歷/書審寫）
- 設定功率門檻：若 `power > THRESHOLD_W` → ALERT
- 或使用移動平均：
  - 計算最近 N 筆平均功率 `avg`
  - 若 `power > avg * 2` → ALERT（突增異常）
- 也可延伸：用電趨勢預測、設備健康度評估

---

## 🧪 成果展示（你可以放截圖）
- Google Sheets 折線圖：power vs time
- status 欄位：NORMAL / ALERT
- （選配）OLED 顯示即時功率與狀態

---

## 🧩 後續擴充（加分）
- LINE Bot 推播異常通知
- 接入 MQTT / InfluxDB / Grafana
- 加入「功率因數 PF」或「諧波」(進階)
- 多點監測：同時監測多個插座/迴路

---

## 📜 License
MIT License


function doPost(e) {
  try {
    const ss = SpreadsheetApp.getActiveSpreadsheet();
    const sheet = ss.getSheets()[0];

    const data = JSON.parse(e.postData.contents);

    const ts = new Date();
    const row = [
      ts,
      data.voltage ?? "",
      data.current ?? "",
      data.power ?? "",
      data.energy ?? "",
      data.status ?? "NORMAL"
    ];

    sheet.appendRow(row);

    return ContentService
      .createTextOutput(JSON.stringify({ ok: true, saved: row }))
      .setMimeType(ContentService.MimeType.JSON);

  } catch (err) {
    return ContentService
      .createTextOutput(JSON.stringify({ ok: false, error: String(err) }))
      .setMimeType(ContentService.MimeType.JSON);
  }
}

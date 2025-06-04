# ESP32 Fingerprint Attendance System with LCD and Google Sheets Integration

## Overview

This project uses an ESP32 microcontroller to read fingerprints using an Adafruit fingerprint sensor, display status and results on a 16x2 I2C LCD, and send fingerprint match data to Google Sheets via a Google Apps Script web app.

---

## Features

- Connects to WiFi and maintains connection.
- Reads fingerprint data and verifies against enrolled fingerprints.
- Displays fingerprint ID and confidence on a 16x2 LCD.
- Sends matching fingerprint data (ID and confidence) to Google Sheets via HTTP POST request.
- Provides visual feedback on LCD for connection, sensor status, and matching results.

---

## Hardware Required

- ESP32 development board  
- Adafruit Fingerprint Sensor (or compatible)  
- 16x2 I2C LCD Display (address usually 0x27)  
- Connecting wires  
- USB cable to program ESP32  
- Breadboard (optional)  

---

## Wiring

| ESP32 Pin | Device                | Notes                     |
|-----------|-----------------------|---------------------------|
| GPIO16    | Fingerprint Sensor TX | ESP32 RX (Serial2 RX)     |
| GPIO17    | Fingerprint Sensor RX | ESP32 TX (Serial2 TX)     |
| GPIO21    | LCD SDA               | I2C SDA (check your board)|
| GPIO22    | LCD SCL               | I2C SCL (check your board)|
| 3.3V      | Fingerprint Sensor VCC| Power                     |
| GND       | Fingerprint Sensor GND| Ground                    |
| 5V or 3.3V| LCD VCC               | Power (depends on LCD)    |
| GND       | LCD GND               | Ground                    |

---

Google Apps Script Code
Create a new Google Apps Script attached to your Google Sheet and paste this code:

function doPost(e) {
  try {
    // Parse the JSON POST data
    var data = JSON.parse(e.postData.contents);
    var id = data.id;
    var confidence = data.confidence;

  
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Sheet1");

  if (!sheet) {
      // If Sheet1 does not exist, create it
      sheet = SpreadsheetApp.getActiveSpreadsheet().insertSheet("Sheet1");
      // Add header row
      sheet.appendRow(["Timestamp", "Fingerprint ID", "Confidence"]);
    }

  
   sheet.appendRow([new Date(), id, confidence]);


  return ContentService
      .createTextOutput(JSON.stringify({status: "success", message: "Data logged"}))
      .setMimeType(ContentService.MimeType.JSON);

  } catch (error) {
    // Handle errors and return failure response
    return ContentService
      .createTextOutput(JSON.stringify({status: "error", message: error.message}))
      .setMimeType(ContentService.MimeType.JSON);
  }
}


Deploy Google Apps Script as Web App
In the Apps Script editor, click Deploy > New deployment.

Select Web app.

Set Execute as: Me (your Google account).

Set Who has access: Anyone.

Click Deploy and authorize permissions.

Copy the Web app URL and replace the GOOGLE_SCRIPT_URL in the Arduino code with this URL.

---

Uploading and Running
Connect your ESP32 to your PC.

Select the correct board and port in Arduino IDE.

Upload the Arduino sketch.

Open Serial Monitor at 115200 baud to see debug messages.

The LCD will show WiFi and sensor status.

Place your finger on the sensor; if a match is found, ID and confidence will show on the LCD, and the data will be sent to Google Sheets.

---

Troubleshooting
Fingerprint sensor not detected: Check wiring and power supply.

WiFi connection fails: Verify SSID and password.

No data in Google Sheets: Check Google Script URL and deployment permissions.

LCD not showing: Verify I2C address and wiring.

---

License
This project is open source and free to use for educational purposes.

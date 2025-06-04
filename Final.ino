combined_code = """\
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fingerprint Sensor Serial (ESP32 Serial2)
HardwareSerial mySerial(2); // RX = GPIO16, TX = GPIO17
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// WiFi Credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Google Script Web App URL (Replace with your own)
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec";

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");

  // Start fingerprint sensor
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  finger.begin(57600);
  delay(100);

  if (finger.verifyPassword()) {
    lcd.setCursor(0, 1);
    lcd.print("Sensor Ready");
    Serial.println("Fingerprint sensor detected.");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Sensor Error");
    Serial.println("Fingerprint sensor not found.");
    while (1) delay(1);
  }
}

void loop() {
  if (getFingerprintID()) {
    delay(2000); // Pause between reads
  }
}

bool getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return false;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Match Found!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID: ");
    lcd.print(finger.fingerID);
    lcd.setCursor(0, 1);
    lcd.print("Conf: ");
    lcd.print(finger.confidence);

    sendToGoogle(finger.fingerID, finger.confidence);
    return true;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Match Found");
    Serial.println("No fingerprint match.");
    return false;
  }
}

void sendToGoogle(uint8_t id, uint16_t conf) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(GOOGLE_SCRIPT_URL);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"id\":" + String(id) + ",\"confidence\":" + String(conf) + "}";
    int httpCode = http.POST(json);
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Google Sheets response: " + response);
    } else {
      Serial.println("Error posting to Google Sheets.");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
"""

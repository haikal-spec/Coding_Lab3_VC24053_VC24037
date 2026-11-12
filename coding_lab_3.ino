#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// =====================================
// WiFi Credentials
// =====================================

const char* ssid = "noorsuhaina79_2.4GHz@unifi";
const char* password = "suhaina@5504";

// =====================================
// Google Apps Script Web App URL
// =====================================

const char* scriptURL = "https://script.google.com/macros/s/AKfycbzppAr2lGZimxBq9CowQjHETVdQS_x0USQv8dr8qKeBqAedtIxqon_fwKpJN4KrVmBMkw/exec";

// =====================================
// DHT11 Sensor Configuration
// =====================================

#define DHTPIN 16
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// =====================================
// Data Sending Interval
// 10 seconds for testing
// =====================================

const unsigned long sendInterval = 600000;

unsigned long previousMillis = 0;

// =====================================
// SETUP
// =====================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("ESP32 DHT11 Sensor Data Logger");

  // Initialize DHT11
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// =====================================
// MAIN LOOP
// =====================================

void loop() {

  unsigned long currentMillis = millis();

  // Check sending interval
  if (currentMillis - previousMillis >= sendInterval) {

    previousMillis = currentMillis;

    // Read temperature and humidity
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check sensor reading
    if (isnan(humidity) || isnan(temperature)) {

      Serial.println("Failed to read from DHT sensor!");

      return;
    }

    // Display sensor readings
    Serial.println("---------------------------");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send data to Google Sheets
    sendDataToGoogleSheets(temperature, humidity);
  }
}

// =====================================
// SEND DATA TO GOOGLE SHEETS
// =====================================

void sendDataToGoogleSheets(float temperature, float humidity) {

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi not connected");

    return;
  }

  HTTPClient http;

  http.begin(scriptURL);

  http.addHeader("Content-Type", "application/json");

  // Create JSON data
  StaticJsonDocument<200> doc;

  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String jsonString;

  serializeJson(doc, jsonString);

  Serial.println("Sending data to Google Sheets...");

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {

    String response = http.getString();

    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    Serial.print("Response: ");
    Serial.println(response);

  } else {

    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

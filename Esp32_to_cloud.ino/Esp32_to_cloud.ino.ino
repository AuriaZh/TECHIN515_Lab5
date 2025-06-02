#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define WIFI_SSID "Lab5_Network"              // Replace with your WiFi SSID
#define WIFI_PASSWORD "SecurePass_2025"       // Replace with your WiFi password

#define SERVER_URL "http://YOUR_SERVER_IP:5000/"  // Replace with your Flask server's public IP + port

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Simulate WiFi connection
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring!");
    while (1);
  }
  Serial.println("MPU6050 initialized.");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Send sensor data to server
  sendRawDataToServer(a.acceleration.x, a.acceleration.y, a.acceleration.z);

  delay(1000);  // Send once per second
}

void sendRawDataToServer(float x, float y, float z) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(256);
    doc["x"] = x;
    doc["y"] = y;
    doc["z"] = z;

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);

      // Parse server response and print
      DynamicJsonDocument respDoc(256);
      DeserializationError error = deserializeJson(respDoc, response);
      if (!error) {
        const char* gesture = respDoc["gesture"];
        float server_confidence = respDoc["confidence"];
        Serial.printf("Gesture: %s, Confidence: %.2f%%\n", gesture, server_confidence);
      } else {
        Serial.println("Failed to parse server response.");
      }

    } else {
      Serial.printf("Error sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}


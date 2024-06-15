#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

const char* ssid = "NHUNG1975";
const char* password = "0971947652";

const char* serverIP = "192.168.1.5";
const int serverPort = 8000;

const char* endpointConnect = "/connect";
const char* endpointGetPassword = "/getPassword";

String serverURLConnect = "http://" + String(serverIP) + ":" + String(serverPort) + endpointConnect;
String serverURLGetPassword = "http://" + String(serverIP) + ":" + String(serverPort) + endpointGetPassword;

String currentPassword;

void connectToWiFi() {
  Serial.print("Connecting to WiFi ..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX2 and TX2 are connected to pins 16 and 17
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sendPostRequest();
    sendGetRequest();
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    connectToWiFi();
  }

  receivePassword();
  
  delay(5000);
}

void sendPostRequest() {
  HTTPClient http;
  http.begin(serverURLConnect);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST("{}");
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("POST Response:");
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void sendGetRequest() {
  HTTPClient http;
  http.begin(serverURLGetPassword);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("GET Response:");
    Serial.println(httpResponseCode);
    Serial.println(response);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error && doc.size() > 0) {
      currentPassword = doc[0]["password"].as<String>();
      Serial.print("Current Password: ");
      Serial.println(currentPassword);
    } else {
      Serial.print("Failed to parse JSON or empty response");
    }
  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void receivePassword() {
  if (Serial2.available()) {
    String receivedPassword = Serial2.readStringUntil('\n');
    receivedPassword.trim();
    Serial.print("Received Password from Arduino: ");
    Serial.println(receivedPassword);
  }
}

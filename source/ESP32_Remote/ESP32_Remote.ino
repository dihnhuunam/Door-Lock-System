#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const char* ssid = "NHUNG1975";
const char* password = "0971947652";

const char* serverIP = "192.168.1.5";
const int serverPort = 8000;

const char* endpointConnect = "/connect";
const char* endpointGetPassword = "/getPassword";

String serverURLConnect = "http://" + String(serverIP) + ":" + String(serverPort) + endpointConnect;
String serverURLGetPassword = "http://" + String(serverIP) + ":" + String(serverPort) + endpointGetPassword;

String currentPassword;
String enteredPassword = "";

// Define SoftwareSerial pins for ESP32
#define RX_PIN 16 // Connect GPIO 16 of ESP32 to D3 of Arduino Uno
#define TX_PIN 17 // Connect GPIO 17 of ESP32 to D2 of Arduino Uno

// SoftwareSerial object for ESP32 to communicate with Arduino Uno
SoftwareSerial espSerial(RX_PIN, TX_PIN);  // RX2 and TX2 are connected to pins 16 and 17

// Check connection with Arduino Uno
bool checkArduinoConnection() {
  espSerial.println("CHECK_CONNECTION");
  int startTime = millis();
  while (millis() - startTime < 2000) { // wait for 2 seconds
    if (espSerial.available()) {
      String response = espSerial.readStringUntil('\n');
      response.trim();
      Serial.print("Response from Arduino: ");
      Serial.println(response);
      if (response == "CONNECTED SUCCESSFULLY") {
        return true;
      }
    }
  }
  return false;
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi ..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
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
      // Send the password to Arduino
      espSerial.println("SET_PASSWORD:" + currentPassword);
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
  while (espSerial.available()) {
    char key = espSerial.read();
    if (key == '\n') {
      // Process the entered password
      Serial.print("Entered Password: ");
      Serial.println(enteredPassword);

      if (enteredPassword == currentPassword) {
        unlockDoor();
      } else {
        Serial.println("Incorrect Password");
        espSerial.println("INCORRECT_PASSWORD");
      }

      enteredPassword = ""; // Reset the entered password
    } else {
      enteredPassword += key; // Add the key to the entered password
    }
  }
}

void unlockDoor() {
  // Send command to Arduino to unlock the door
  espSerial.println("UNLOCK");
  Serial.println("Unlocking door...");
}

void setup() {
  Serial.begin(115200); // Initialize Serial for debugging
  espSerial.begin(9600); // Initialize SoftwareSerial communication with Arduino
  connectToWiFi(); // Connect to WiFi
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sendPostRequest();
    sendGetRequest();
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    connectToWiFi();
  }

  if (!checkArduinoConnection()) {
    Serial.println("Arduino Disconnected. Attempting to reconnect...");
    // Implement reconnection logic if needed
  } else {
    Serial.println("Arduino is connected.");
  }

  receivePassword();
  delay(5000); // Wait for 5 seconds before next iteration
}

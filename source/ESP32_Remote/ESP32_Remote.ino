#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const char* ssid = "NHUNG1975";
const char* password = "0971947652";

const char* serverIP = "192.168.1.3";
const int serverPort = 8000;

const char* endpointConnect = "/connect";
const char* endpointGetPassword = "/getPassword";
const char* endpointChangePassword = "/changePassword";
const char* endpointCheckDoorStatus = "/doorStatus";  // New endpoint to check door status

String serverURLConnect = "http://" + String(serverIP) + ":" + String(serverPort) + endpointConnect;
String serverURLGetPassword = "http://" + String(serverIP) + ":" + String(serverPort) + endpointGetPassword;
String serverURLChangePassword = "http://" + String(serverIP) + ":" + String(serverPort) + endpointChangePassword;
String serverURLCheckDoorStatus = "http://" + String(serverIP) + ":" + String(serverPort) + endpointCheckDoorStatus;

String currentPassword;
String enteredPassword = "";

// Define SoftwareSerial pins for ESP32
#define RX_PIN 16 // Connect GPIO 16 of ESP32 to D3 of Arduino Uno
#define TX_PIN 17 // Connect GPIO 17 of ESP32 to D2 of Arduino Uno

// Connect ESP32 to Arduino Uno
SoftwareSerial espSerial(RX_PIN, TX_PIN);

void connectToWiFi();
void sendPostRequest();
void sendGetRequest();
void sendPasswordChangeRequest(String oldPassword, String newPassword);
void receivePassword();
void unlockDoor();
void checkDoorStatus();

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600); // SoftwareSerial communication with Arduino
  connectToWiFi(); 
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sendPostRequest();
    sendGetRequest();
    checkDoorStatus();
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    connectToWiFi();
  }

  receivePassword();
  delay(5000);
  Serial.println("---------------------------------------------");
}

// Connect to WiFi
void connectToWiFi() {
  Serial.print("Connecting to WiFi ..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
}

// Send a POST request
void sendPostRequest() {
  HTTPClient http;
  http.begin(serverURLConnect);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST("{}");
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("POST Response: ");
    Serial.print(httpResponseCode);
    Serial.print(": ");
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Send a GET request
void sendGetRequest() {
  HTTPClient http;
  http.begin(serverURLGetPassword);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("GET Response: ");
    Serial.print(httpResponseCode);
    Serial.print(": ");
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
      Serial.println("Failed to parse JSON or empty response");
    }
  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Send a password change request
void sendPasswordChangeRequest(String oldPassword, String newPassword) {
  Serial.print("Old Password from Arduino: ");
  Serial.println(oldPassword);
  Serial.print("Current Password stored: ");
  Serial.println(currentPassword);

  if (oldPassword != currentPassword) {
    Serial.println("Entered old password does not match the current password.");
    espSerial.println("INCORRECT_OLD_PASSWORD");
    return;
  }

  HTTPClient http;
  http.begin(serverURLChangePassword);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;
  doc["oldPassword"] = oldPassword;
  doc["newPassword"] = newPassword;

  String requestBody;
  serializeJson(doc, requestBody);
  Serial.print("Request Body: ");
  Serial.println(requestBody);  // Debug request body

  int httpResponseCode = http.POST(requestBody);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Password Change Response: ");
    Serial.print(httpResponseCode);
    Serial.print(": ");
    Serial.println(response);

    StaticJsonDocument<200> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error) {
      Serial.print("Parsed JSON: ");
      serializeJson(responseDoc, Serial);  // Print the entire parsed JSON document
      Serial.println();
      const char* message = responseDoc["message"];
      if (message && strcmp(message, "Password changed successfully") == 0) {
        currentPassword = newPassword;  // Update the current password
        espSerial.println("SET_PASSWORD:" + newPassword);  // Update the Arduino
        Serial.println("Password changed successfully");
      } else {
        Serial.println("Password change failed");
        espSerial.println("PASSWORD_CHANGE_FAILED");
      }
    } else {
      Serial.print("Failed to parse JSON response: ");
      Serial.println(error.c_str());
      Serial.println(response);  // Print the raw response for debugging
    }
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Receive data from Arduino
void receivePassword() {
  while (espSerial.available()) {
    String receivedData = espSerial.readStringUntil('\n');
    receivedData.trim();
    Serial.print("Received from Arduino: ");
    Serial.println(receivedData);

    if (receivedData.startsWith("CHANGE_PASSWORD:")) {
      int firstColonIndex = receivedData.indexOf(':');
      int secondColonIndex = receivedData.indexOf(':', firstColonIndex + 1);

      String oldPassword = receivedData.substring(firstColonIndex + 1, secondColonIndex);
      String newPassword = receivedData.substring(secondColonIndex + 1);

      sendPasswordChangeRequest(oldPassword, newPassword);
    } else {
      enteredPassword += receivedData;
      if (receivedData.endsWith("\n")) {
        enteredPassword.trim();
        if (enteredPassword == currentPassword) {
          unlockDoor();
        } else {
          Serial.println("Incorrect Password");
          espSerial.println("INCORRECT_PASSWORD");
        }
        enteredPassword = "";
      }
    }
  }
}

// Function to unlock the door
void unlockDoor() {
  // Send command to Arduino to unlock the door
  espSerial.println("UNLOCK");
  Serial.println("Unlocking door...");
}

// New function to check the door status from server
void checkDoorStatus() {
  HTTPClient http;
  http.begin(serverURLCheckDoorStatus);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Check Door Status Response: ");
    Serial.print(httpResponseCode);
    Serial.print(": ");
    Serial.println(response);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      const char* doorStatus = doc["status"];
      if (strcmp(doorStatus, "open") == 0) {
        unlockDoor();
      }
    } else {
      Serial.println("Failed to parse JSON response");
    }
  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

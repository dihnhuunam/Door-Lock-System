#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SoftwareSerial.h>

// WiFi credentials
const char* ssid = "NHUNG1975";
const char* password = "0971947652";

// Unlock password
const char* unlockPassword = "1234";

// Create a web server on port 80
WebServer server(80);

// Define SoftwareSerial pins for ESP32
#define RX_PIN 16 // Connect GPIO 16 of ESP32 to D3 of Arduino Uno
#define TX_PIN 17 // Connect GPIO 17 of ESP32 to D2 of Arduino Uno

// Create SoftwareSerial object for ESP32 communication with Arduino Uno
SoftwareSerial espSerial(RX_PIN, TX_PIN);

// Function to handle root URL (handle html)
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open index.html");
    return;
  }
  String html = file.readString();
  server.send(200, "text/html", html);
  file.close();
}

// Function to handle CSS requests
void handleCSS() {
  File file = SPIFFS.open("/styles.css", "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open styles.css");
    return;
  }
  String css = file.readString();
  server.send(200, "text/css", css);
  file.close();
}

// Function to handle unlock request
void handleUnlock() {
  if (server.hasArg("password")) {
    String password = server.arg("password");
    if (password.equals(unlockPassword)) {
      // Send unlock command to Arduino
      espSerial.println("UNLOCK");
      server.send(200, "text/plain", "Door unlocked successfully!");
    } else {
      server.send(401, "text/plain", "Incorrect password. Please try again.");
    }
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

// Function to check the connection with Arduino
bool checkConnection() {
  espSerial.println("CHECK_CONNECTION");
  int startTime = millis();
  while (millis() - startTime < 2000) { // wait for 2 seconds
    if (espSerial.available()) {
      String response = espSerial.readStringUntil('\n');
      response.trim();
      Serial.print("Response from Arduino: ");
      Serial.println(response);
      if (response == "CONNECTED SUCCESSFULY") {
        return true;
      }
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200); // Set baud rate to match with Arduino Uno
  Serial.println("ESP32 starting...");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully");

  // Initialize SoftwareSerial
  espSerial.begin(9600); // Set baud rate for SoftwareSerial
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start web server
  server.on("/", handleRoot);
  server.on("/styles.css", handleCSS);
  server.on("/unlock", HTTP_POST, handleUnlock);
  server.begin();
  Serial.println("HTTP server started");

  // Delay to ensure Arduino is ready
  delay(5000);

  // Check connection with Arduino
  if (checkConnection()) {
    Serial.println("Connection with Arduino successful");
  } else {
    Serial.println("Failed to connect to Arduino");
  }
}

void loop() {
  server.handleClient();
}

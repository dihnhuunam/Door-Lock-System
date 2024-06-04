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
#define RX_PIN 16  // Connect GPIO 16 of ESP32 to D3 of Arduino Uno
#define TX_PIN 17  // Connect GPIO 17 of ESP32 to D2 of Arduino Uno

//  SoftwareSerial object for ESP32 to communicate with Arduino Uno
SoftwareSerial espSerial(RX_PIN, TX_PIN);

// Connect to WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Initialize SPIFFS
void initializeSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully");
}

// Handle root URL (serve index.html)
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
// Handle CSS requests (serve styles.css)
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
// Handle unlock request
void handleUnlock() {
  if (server.hasArg("password")) {
    String password = server.arg("password");
    if (password.equals(unlockPassword)) {
      // Send unlock command to Arduino
      espSerial.println("UNLOCK");
      server.send(200, "text/plain", "Door unlocked successfully!");
    } 
    else {
      server.send(401, "text/plain", "Incorrect password. Please try again.");
    }
  } 
  else {
    server.send(400, "text/plain", "Bad Request");
  }
}
// Start the WebServer
void startWebServer() {
  // Start web server
  server.on("/", handleRoot);
  server.on("/styles.css", handleCSS);
  server.on("/unlock", HTTP_POST, handleUnlock);
  server.begin();
  Serial.println("HTTP server started");
}

// Check connection with Arduino Uno
bool checkArduinConnection() {
  espSerial.println("CHECK_CONNECTION");
  int startTime = millis();
  while (millis() - startTime < 2000) {  // wait for 2 seconds
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

void checkWiFiConnection() {
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(10000);
}

void setup() {
  Serial.begin(115200);  
  Serial.println("ESP32 starting...");
  espSerial.begin(9600);  // SoftwareSerial at baudrate 9600

  connectToWiFi();
  initializeSPIFFS();
  startWebServer();
  delay(5000);

  // Check connection with Arduino
  if (checkArduinConnection()) {
    Serial.println("Connection with Arduino successful");
  } 
  else {
    Serial.println("Failed to connect to Arduino");
  }
}

void loop() {
  server.handleClient();  // Handle client requests
  checkWiFiConnection();
}

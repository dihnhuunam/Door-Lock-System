#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

// WiFi credentials
const char* ssid = "NHUNG1975";
const char* password = "0971947652";

// Unlock password
const char* unlockPassword = "1234";

// Create a web server on port 80
WebServer server(80);

// Function to handle root URL
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
      Serial.println("UNLOCK");
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
  Serial.println("CHECK_CONNECTION");
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) { // wait for 2 seconds
    if (Serial.available()) {
      String response = Serial.readStringUntil('\n');
      response.trim();
      Serial.print("Response from Arduino: ");
      Serial.println(response);
      if (response == "CONNECTION_OK") {
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

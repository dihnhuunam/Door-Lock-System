#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "tracalaka";
const char* password = "20213626";
const char* unlockPassword = "1234";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<!-- Paste your HTML content here -->");
}

void handleUnlock() {
  if (server.hasArg("password")) {
    String password = server.arg("password");
    if (password.equals(unlockPassword)) {
      // Code to unlock
      server.send(200, "text/plain", "success");
    } else {
      server.send(401, "text/plain", "Unauthorized");
    }
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start web server
  server.on("/", handleRoot);
  server.on("/unlock", HTTP_POST, handleUnlock);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

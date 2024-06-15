#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Include ArduinoJson library

// Replace with your network credentials
const char* ssid = "NHUNG1975";
const char* password = "0971947652";

// Server IP Address and Port
const char* serverIP = "192.168.1.5";
const int serverPort = 8000;

// Endpoint/Paths
const char* endpointConnect = "/connect";
const char* endpointGetPassword = "/getPassword"; // Example endpoint for GET request

// Construct the full server URLs
String serverURLConnect = "http://" + String(serverIP) + ":" + String(serverPort) + endpointConnect;
String serverURLGetPassword = "http://" + String(serverIP) + ":" + String(serverPort) + endpointGetPassword;

// Global variable to store password
String currentPassword;

// Function prototypes
void connectToWiFi();
void sendPostRequest();
void sendGetRequest();

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Check if connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {
    // Example: Send POST request to server
    sendPostRequest();
    
    // Example: Send GET request to server
    sendGetRequest();
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    // Attempt to reconnect to WiFi
    connectToWiFi();
  }

  // Wait for 5 seconds
  delay(5000);
}

void connectToWiFi() {
  // Connect to Wi-Fi network
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
  http.begin(serverURLConnect); // Use the constructed server URL for POST
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST("{}"); // Send empty JSON payload
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
  http.begin(serverURLGetPassword); // Use the constructed server URL for GET

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("GET Response:");
    Serial.println(httpResponseCode);
    Serial.println(response);

    // Parse JSON response
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error && doc.size() > 0) {
      // Extract password from JSON and store in global variable
      storedPassword = doc[0]["password"].as<String>();
      Serial.print("Stored Password: ");
      Serial.println(storedPassword);
    } else {
      Serial.print("Failed to parse JSON or empty response");
    }
  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

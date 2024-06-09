#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "NHUNG1975";
const char* password = "0971947652";

const char* serverUrl = "http://localhost:8000/";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Replace with actual password input logic
    String payload = "{\"password\":\"1234\"}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);

      if (response == "Password is correct, door unlocked") {
        // Code to unlock the door (e.g., move servo motor)
        Serial.println("Unlocking door...");
      } else {
        Serial.println("Access denied");
      }
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(10000); // Delay for demonstration purposes
}

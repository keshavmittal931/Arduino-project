#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // Required for HTTPS connections

#define MQ5_PIN A0    // MQ-5 sensor input pin
#define BUZZER_PIN D2 // Buzzer output pin
#define RELAY_PIN D1  // Relay control pin

// Replace with your WiFi credentials
const char* ssid = "Redmi";
const char* password = "12345678";

// Replace with your API Key from Circuit Digest Cloud
const char* apiKey = "QU0E9VpSaSXU"; // Your API key from Circuit Digest Cloud

// SMS details (users can easily modify these)
const char* templateID = "110";                  // Template ID
const char* mobileNumber = "918708942668";       // Mobile number (with country code)
const char* var1 = "is LEAKING GASS";         // Variable 1
const char* var2 = "111100001"; // Variable 2

int gasValue = 0;  // Value read from MQ-5 sensor
int threshold = 150;  // Set threshold for gas concentration detection

WiFiClientSecure client; // Secure client for HTTPS requests
HTTPClient http;         // HTTP client for sending SMS requests

void setup() {
  Serial.begin(115200); // Start serial communication

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Set pin modes
  pinMode(MQ5_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Initial state for relay and buzzer (off)
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // Read gas sensor value (analog input from MQ-5)
  gasValue = analogRead(MQ5_PIN);

  // If gas level exceeds threshold, trigger alarm and notification
  if (gasValue > threshold) {
    Serial.println("Gas Leak Detected!");

    // Turn on Buzzer
    digitalWrite(BUZZER_PIN, HIGH);

    // Activate Relay (e.g., turn off gas supply or activate safety mechanism)
    digitalWrite(RELAY_PIN, HIGH);

    // Send SMS via Circuit Digest Cloud API
    sendSMS();

    // Wait a bit before checking again
    delay(2000);
  } else {
    // Turn off Buzzer and Relay if no leak is detected
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
  }

  delay(1000);  // Delay between sensor reads to avoid overloading
}

void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    client.setInsecure();    // Skip certificate validation (not secure but works for development)

    // Build the API URL with the template ID
    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);

    // Start the HTTPS connection with WiFiClientSecure
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload with SMS details
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + String(var1) + "\",\"var2\":\"" + String(var2) + "\"}";

    // Send POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode == 200) {
      Serial.println("SMS sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("Failed to send SMS. Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + http.getString());
    }

    http.end(); // End connection
  } else {
    Serial.println("WiFi not connected!");
  }
}

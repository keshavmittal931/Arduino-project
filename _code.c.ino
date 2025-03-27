#include <SoftwareSerial.h>


SoftwareSerial BTSerial(10, 11);

// Pin Definitions
#define MQ5_PIN A0         // MQ5 sensor analog pin
#define RELAY_PIN 7        // Relay module pin
#define BUZZER_PIN 8       // Buzzer pin
#define GAS_THRESHOLD 250  // Adjust threshold based on testing

void setup() {
  // Pin Modes
  pinMode(MQ5_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initial State
  digitalWrite(RELAY_PIN, HIGH); // Relay initially ON
  digitalWrite(BUZZER_PIN, LOW); // Buzzer initially OFF

  // Serial and Bluetooth Initialization
  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("System Initialized. Monitoring for Gas Leaks...");
}

void loop() {
  int gasValue = analogRead(MQ5_PIN); // Read gas sensor value
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  if (gasValue > GAS_THRESHOLD) {
    // Gas Leak Detected: Turn OFF Relay, Turn ON Buzzer, Send Notification
    digitalWrite(RELAY_PIN, LOW);  
    digitalWrite(BUZZER_PIN, HIGH); 
    Serial.println("Gas Leak Detected! Turning off relay & activating buzzer.");
    BTSerial.println("ALERT: Gas Leak Detected! Relay OFF, Buzzer ON.");
  } else {
    // No Gas Leak: Keep Relay ON, Buzzer OFF
    digitalWrite(RELAY_PIN, HIGH); 
    digitalWrite(BUZZER_PIN, LOW); 
    Serial.println("No Gas Leak Detected. System Normal.");
    BTSerial.println("Status: No Gas Leak Detected. System Normal.");
  }

  delay(1000); // 1-second delay for stability
}

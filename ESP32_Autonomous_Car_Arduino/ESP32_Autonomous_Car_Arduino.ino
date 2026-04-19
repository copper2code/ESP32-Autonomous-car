#include <WiFi.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

const char* ssid = "Tiklehouse";
const char* password = "Pass@1660";



WiFiServer server(12345);

Servo myServo;

const int servoPin = 13;
const int motorPwmPin = 19;
const int motorDir1 = 5;
const int motorDir2 = 18;
const int ledPin = 2; // Onboard LED

// Motor PWM properties
const int pwmChannel = 0;
const int pwmFreq = 50;
const int pwmResolution = 10; // 0-1023

void setup() {
  Serial.begin(115200);
  
  pinMode(motorDir1, OUTPUT);
  pinMode(motorDir2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Setup Motor PWM
  ledcAttach(motorPwmPin, pwmFreq, pwmResolution);
  
  // Setup Servo
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400); // Attach servo
  
  // Connect to Wi-Fi
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Turn on onboard LED for 5 sec to notify successful connection
  digitalWrite(ledPin, HIGH);
  delay(5000);
  digitalWrite(ledPin, LOW);
  
  server.begin();
  Serial.println("Server started on port 12345");
}

void setMotorSpeed(int speed) {
  int pwmVal = constrain(abs(speed), 0, 1023);
  ledcWrite(motorPwmPin, pwmVal);
  if (speed > 0) {
    digitalWrite(motorDir1, HIGH);
    digitalWrite(motorDir2, LOW);
  } else if (speed < 0) {
    digitalWrite(motorDir1, LOW);
    digitalWrite(motorDir2, HIGH);
  } else {
    digitalWrite(motorDir1, LOW);
    digitalWrite(motorDir2, LOW);
  }
}

// Convert the MicroPython duty cycle (out of 1023) to Servo angle (degrees).
// 55 represents 5.3% duty (~1ms) -> angle 45 (or 0)
// 89 represents 8.6% duty (~2ms) -> angle 135 (or 180)
int mapDutyToAngle(int duty) {
    return constrain(map(duty, 55, 89, 45, 135), 0, 180);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Got connection from client");
    setMotorSpeed(0);
    
    // Send welcome message
    client.print("thank you for connection");
    
    while (client.connected()) {
      if (client.available()) {
        char buffer[1024];
        memset(buffer, 0, 1024);
        int bytesRead = client.read((uint8_t*)buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
          Serial.print("Received: ");
          Serial.println(buffer);
          
          // Use ArduinoJson to decode the payload
          StaticJsonDocument<256> doc;
          DeserializationError error = deserializeJson(doc, buffer);
          
          if (!error) {
            int count = doc.size();
            // Depending on how many keys we get, we follow the old script logic
            if (count < 3) {
              int a = doc["a"]; // angle parameter
              int w = doc["w"]; // speed parameter
              
              myServo.write(mapDutyToAngle(a));
              setMotorSpeed(w);
              Serial.printf("Angle: %d, Speed: %d\n", a, w);
            } else {
              int w = doc["w"];
              int s = doc["s"];
              int a = doc["a"];
              int d = doc["d"];
              
              if (w == 1 && s == 0) setMotorSpeed(1000);
              else if (s == 1 && w == 0) setMotorSpeed(-1000);
              else if (w == 0 && s == 0) setMotorSpeed(0);
              
              if (a == 1 && d == 0) myServo.write(mapDutyToAngle(55));
              else if (d == 1 && a == 0) myServo.write(mapDutyToAngle(89));
              else if (d == 0 && a == 0) myServo.write(mapDutyToAngle(75));
            }
          } else {
            Serial.print("JSON deserialization failed: ");
            Serial.println(error.c_str());
          }
          
          // IMPORTANT: We must send a reply back to the client!
          // The Python client's s.recv(1024) is a blocking call and will freeze 
          // if we don't return data after every transaction.
          client.print("thank you for connection");
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
    setMotorSpeed(0);
  }
}

#include <ArduinoMqttClient.h>

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) ||  \
    defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) ||      \
    defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) ||                    \
    defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
#include <WiFiS3.h>
#endif

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
byte mac[6]; // WiFi MAC address

// Motor & Control Pins
int motor1_forward = 1;
int motor1_backward = 2;
int motor1_speed = 3;

int motor2_forward = 4;
int motor2_backward = 5;
int motor2_speed = 6;

int motor3_left = 7;
int motor3_right = 8;
int motor3_speed = 9;

int status_led = LED_BUILTIN;

int kicker_forward = 12;
int kicker_speed = 10;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "your-broker-ip-or-domain";
int port = 1883;
const char topic[] = "rover/control"; // MQTT topic

// Speed control state
String lastCommand = "";
unsigned long lastCommandTime = 0;
int repeatCount = 0;
int baseSpeed = 160;
int maxSpeed = 255;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  pinMode(motor1_forward, OUTPUT);
  pinMode(motor1_backward, OUTPUT);
  pinMode(motor1_speed, OUTPUT);
  pinMode(motor2_forward, OUTPUT);
  pinMode(motor2_backward, OUTPUT);
  pinMode(motor2_speed, OUTPUT);
  pinMode(motor3_left, OUTPUT);
  pinMode(motor3_right, OUTPUT);
  pinMode(motor3_speed, OUTPUT);
  pinMode(status_led, OUTPUT);
  pinMode(kicker_forward, OUTPUT);
  pinMode(kicker_speed, OUTPUT);

  Serial.println("=== Arduino Network & MQTT Debug ===");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  WiFi.macAddress(mac);
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16)
      Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println();

  // Connectivity test
  Serial.println("Testing TCP connection to broker...");
  WiFiClient testClient;
  testClient.setTimeout(10000);
  if (testClient.connect(broker, port)) {
    Serial.println("TCP Connection: SUCCESS!");
    testClient.stop();
  } else {
    Serial.println("TCP Connection: FAILED!");
    Serial.println("Trying Google DNS...");
    if (testClient.connect("8.8.8.8", 53)) {
      Serial.println("Internet connectivity OK.");
      testClient.stop();
    } else {
      Serial.println("No internet connectivity.");
      while (1)
        delay(1000);
    }
  }

  mqttClient.setConnectionTimeout(15000);
  mqttClient.setKeepAliveInterval(60000);

  String clientId = "Arduino-" + String(random(10000, 99999));
  mqttClient.setId(clientId.c_str());

  Serial.print("MQTT Client ID: ");
  Serial.println(clientId);

  Serial.print("Connecting to MQTT broker ");
  Serial.print(broker);
  Serial.print(":");
  Serial.println(port);

  if (mqttClient.connect(broker, port)) {
    Serial.println("MQTT Connection: SUCCESS!");
    mqttClient.subscribe(topic);
    mqttClient.beginMessage("rover/status");
    mqttClient.print("Arduino connected successfully!");
    mqttClient.endMessage();
  } else {
    Serial.print("MQTT Connection: FAILED (");
    Serial.print(mqttClient.connectError());
    Serial.println(")");
    while (1)
      delay(1000);
  }

  mqttClient.onMessage(onMqttMessage);
  Serial.println("=== Setup Complete ===");
}

void loop() {
  if (!mqttClient.connected()) {
    Serial.println("MQTT disconnected. Attempting reconnect...");
    if (mqttClient.connect(broker, port)) {
      Serial.println("Reconnected!");
      mqttClient.subscribe(topic);
    } else {
      Serial.print("Reconnect failed: ");
      Serial.println(mqttClient.connectError());
      delay(5000);
    }
  }

  mqttClient.poll();

  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 30000) {
    mqttClient.beginMessage("rover/heartbeat");
    mqttClient.print(millis());
    mqttClient.endMessage();
    lastHeartbeat = millis();
  }
}

void stopMotors() {
  digitalWrite(motor1_forward, LOW);
  digitalWrite(motor1_backward, LOW);
  digitalWrite(motor2_forward, LOW);
  digitalWrite(motor2_backward, LOW);
  digitalWrite(motor3_left, LOW);
  digitalWrite(motor3_right, LOW);
  analogWrite(motor1_speed, 0);
  analogWrite(motor2_speed, 0);
  analogWrite(motor3_speed, 0);
}

void kickBall() {
  digitalWrite(kicker_forward, HIGH);
  analogWrite(kicker_speed, 180);
  delay(200);
  digitalWrite(kicker_forward, LOW);
  analogWrite(kicker_speed, 0);
}

void onMqttMessage(int messageSize) {
  String message = "";
  while (mqttClient.available()) {
    char c = (char)mqttClient.read();
    message += c;
  }

  message.toLowerCase();
  Serial.print("📩 Command received: ");
  Serial.println(message);

  // Handle repeated command speed boost
  unsigned long now = millis();
  if (message == lastCommand && (now - lastCommandTime < 2000)) {
    repeatCount++;
  } else {
    repeatCount = 1;
  }
  lastCommand = message;
  lastCommandTime = now;

  // Calculate dynamic speed
  int speedBoost = baseSpeed;

  stopMotors();

  if (message == "forward") {
    digitalWrite(motor1_forward, HIGH);
    digitalWrite(motor1_backward, LOW);
    digitalWrite(motor2_forward, HIGH);
    digitalWrite(motor2_backward, LOW);
    analogWrite(motor1_speed, speedBoost);
    analogWrite(motor2_speed, speedBoost);
    Serial.println("🔼 Moving forward");
  } else if (message == "backward") {
    digitalWrite(motor1_forward, LOW);
    digitalWrite(motor1_backward, HIGH);
    digitalWrite(motor2_forward, LOW);
    digitalWrite(motor2_backward, HIGH);
    analogWrite(motor1_speed, speedBoost);
    analogWrite(motor2_speed, speedBoost);
    Serial.println("🔽 Moving backward");
  } else if (message == "rotate-left") {
    digitalWrite(motor1_forward, HIGH);
    digitalWrite(motor1_backward, LOW);
    digitalWrite(motor2_forward, LOW);
    digitalWrite(motor2_backward, HIGH);
    digitalWrite(motor3_left, LOW);
    digitalWrite(motor3_right, HIGH);
    analogWrite(motor1_speed, speedBoost);
    analogWrite(motor2_speed, speedBoost);
    analogWrite(motor3_speed, speedBoost);
    Serial.println("⟲ Rotating left");
  } else if (message == "rotate-right") {
    digitalWrite(motor1_forward, LOW);
    digitalWrite(motor1_backward, HIGH);
    digitalWrite(motor2_forward, HIGH);
    digitalWrite(motor2_backward, LOW);
    digitalWrite(motor3_left, HIGH);
    digitalWrite(motor3_right, LOW);
    analogWrite(motor1_speed, speedBoost);
    analogWrite(motor2_speed, speedBoost);
    analogWrite(motor3_speed, speedBoost);
    Serial.println("⟳ Rotating right");
  } else if (message == "left") {
    digitalWrite(motor1_forward, HIGH);
    digitalWrite(motor1_backward, LOW);
    digitalWrite(motor2_forward, LOW);
    digitalWrite(motor2_backward, HIGH);
    digitalWrite(motor3_left, HIGH);
    digitalWrite(motor3_right, LOW);
    analogWrite(motor1_speed, speedBoost * 0.6);
    analogWrite(motor2_speed, speedBoost * 0.6);
    analogWrite(motor3_speed, speedBoost);
    Serial.println("↩️ Moving left (reduced motor2/motor3)");
  } else if (message == "right") {
    digitalWrite(motor1_forward, LOW);
    digitalWrite(motor1_backward, HIGH);
    digitalWrite(motor2_forward, HIGH);
    digitalWrite(motor2_backward, LOW);
    digitalWrite(motor3_left, LOW);
    digitalWrite(motor3_right, HIGH);

    analogWrite(motor1_speed, speedBoost * 0.6);
    analogWrite(motor2_speed, speedBoost * 0.6);
    analogWrite(motor3_speed, speedBoost);
    Serial.println("↪️ Moving right (reduced motor2/motor3)");
  } else if (message == "stop") {
    stopMotors();
    Serial.println("⏹️ Motors stopped");
  } else if (message == "kicker") {
    kickBall();
    Serial.println("⚽ Kicker activated");
  } else if (message == "gas") {
    analogWrite(motor1_speed, maxSpeed);
    analogWrite(motor2_speed, maxSpeed);
    Serial.println("🚀 Boost mode");
  } else {
    Serial.print("❓ Unknown command: ");
    Serial.println(message);
  }
}
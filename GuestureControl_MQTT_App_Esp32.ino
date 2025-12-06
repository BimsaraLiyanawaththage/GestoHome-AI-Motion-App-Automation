#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "M01s";
const char* password = "20020503";

const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

#define PIR_PIN 14
#define LIGHT_RELAY_PIN 5
#define FAN_RELAY_PIN 18 

unsigned long lastMotionTime = 0;
unsigned long noMotionLimit = 10000; // 30 seconds waiting time used for testing

bool autoMode = true;         // Motion controls LIGHT automatically
bool lightManual = false;     // Manual for light
bool fanManual = false;       // Manual for fan

bool alertMode = false;       // Waiting user to decide after no motion


void callback(char* topic, byte* message, unsigned int length) {
  String payload = "";
  for (int i = 0; i < length; i++) payload += (char)message[i];

  // For LIGHT OFF
  if (String(topic) == "home/room1/light/off" && payload == "OFF") {
    digitalWrite(LIGHT_RELAY_PIN, LOW);
    lightManual = true;       // User forced state
    autoMode = false;
    alertMode = false;
  }

  // For LIGHT ON
  if (String(topic) == "home/room1/light/on" && payload == "ON") {
    digitalWrite(LIGHT_RELAY_PIN, HIGH);
    lightManual = true;
    autoMode = false;
    alertMode = false;
  }

  // For SYSTEM RESET (Again back to AUTO mode) Motion detection active again
  if (String(topic) == "home/room1/system/reset" && payload == "RESET") {
    autoMode = true;
    lightManual = false;
    alertMode = false;
    lastMotionTime = millis();
  }

  // For FAN ON
  if (String(topic) == "home/room1/fan/on" && payload == "ON") {
    digitalWrite(FAN_RELAY_PIN, HIGH);
    fanManual = true;
  }

  // For FAN OFF
  if (String(topic) == "home/room1/fan/off" && payload == "OFF") {
    digitalWrite(FAN_RELAY_PIN, LOW);
    fanManual = true;
  }
}


void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);

  digitalWrite(LIGHT_RELAY_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}


void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32_motion_device_new")) {

      // Light control topics
      client.subscribe("home/room1/light/on");
      client.subscribe("home/room1/light/off");

      // System reset
      client.subscribe("home/room1/system/reset");

      // Fan control topics (NEW)
      client.subscribe("home/room1/fan/on");
      client.subscribe("home/room1/fan/off");

    } else {
      delay(500);
    }
  }
}


void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int motion = digitalRead(PIR_PIN);

  // For Ato Light Controlling
  if (autoMode && !lightManual) {

    if (motion == HIGH) {
      client.publish("home/room1/motion", "MOTION");

      if (alertMode) {
        alertMode = false;
        client.publish("home/room1/alert", "CLEAR");
      }

      digitalWrite(LIGHT_RELAY_PIN, HIGH);
      lastMotionTime = millis();
    }

    else {
      client.publish("home/room1/motion", "NO_MOTION");

      if (!alertMode && (millis() - lastMotionTime > noMotionLimit)) {
        alertMode = true;
        client.publish("home/room1/alert", "NO_MOTION");
      }
    }
  }
}

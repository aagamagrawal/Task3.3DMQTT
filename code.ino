#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Agam's iPhone";
const char* password = "AgamAgarwal";

// MQTT broker details
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

// MQTT topics
const char* publish_topic = "SIT210/wave";
const char* subscribe_wave_topic = "SIT210/wave";
const char* subscribe_pat_topic = "SIT210/pat";

// Ultrasonic sensor pins
const int trigPin = 7;  // D7
const int echoPin = 6;  // D6

// LED pin
const int ledPin = 8;   // D8

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Check the topic and handle messages accordingly
  if (strcmp(topic, subscribe_wave_topic) == 0) {
    // Handle "wave" messages
    Serial.println("Wave detected!");
    flashLED(3); // Flash LED 3 times
  } else if (strcmp(topic, subscribe_pat_topic) == 0) {
    // Handle "pat" messages
    Serial.println("Pat detected!");
    alternateFlash(); // Flash LED alternately
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoClient")) {
      Serial.println("connected");
      client.subscribe(subscribe_wave_topic);
      client.subscribe(subscribe_pat_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.subscribe(subscribe_wave_topic);
  client.subscribe(subscribe_pat_topic);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Ultrasonic sensor code
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // If wave detected, publish message
  if (distance < 10) { // Adjust this value as needed
    Serial.println("Wave detected!");
    client.publish(publish_topic, "Agam is here");
    flashLED(1); // Flash LED once
    delay(1000); // Publish only once every second
  }
}

// Function to flash the LED a specified number of times
void flashLED(int flashes) {
  for (int i = 0; i < flashes; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

// Function to flash the LED alternately
void alternateFlash() {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
}

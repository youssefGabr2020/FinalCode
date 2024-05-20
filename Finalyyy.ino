#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Youssef's iPhone";
const char* password = "Bccis@2020";

// ThingSpeak settings
const char* thingSpeakAddress = "api.thingspeak.com";
const char* apiKey = "AZTNIIMG06PVGRHM";

// ThingSpeak channel ID
const long CHANNEL_ID = 2531337;

// Pin configuration
const int trigPin = D1;   // Pin for the ultrasonic sensor's trigger pin
const int echoPin = D2;   // Pin for the ultrasonic sensor's echo pin
const int heartRatePin = A0; // Pin for the heart rate sensor
const int DHTPIN = D4;    // Pin for the DHT11 sensor
const int DHTTYPE = DHT11; // Type of DHT sensor
long duration;
int distance;

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client; // Define the WiFi client

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read distance from ultrasonic sensor
  long duration, distance_cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance_cm = duration * 0.034 / 2;

  // Read heart rate from sensor
  int heartRate = analogRead(heartRatePin);
  heartRate = heartRate / 6; // Divide the heart rate by 5

  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Print sensor data for debugging
  Serial.print("Distance (cm): ");
  Serial.println(distance_cm);
  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Update ThingSpeak channel with sensor data
  ThingSpeak.setField(1, String(distance_cm).c_str());
  ThingSpeak.setField(2, String(heartRate).c_str());
  ThingSpeak.setField(3, String(temperature).c_str());
  ThingSpeak.setField(4, String(humidity).c_str());
  
  int httpCode = ThingSpeak.writeFields(CHANNEL_ID, apiKey);
  if (httpCode == 200) {
    Serial.println("Update successful");
  } else {
    Serial.println("Update failed");
  }

  // Wait for 20 seconds before updating again
  delay(20000);
}

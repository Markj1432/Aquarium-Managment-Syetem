#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "Juresh 2.4G";
const char* password = "Liksud9766";
#define ONE_WIRE_BUS 23
#define DHTPIN 5
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 2
#define SOIL_MOISTURE_POWER 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  sensors.begin();
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFaii :3!!");
  }
  Serial.println("Connected to network");

  // Initialize soil moisture sensor power pin
  pinMode(SOIL_MOISTURE_POWER, OUTPUT);
  digitalWrite(SOIL_MOISTURE_POWER, LOW); // Keep sensor off initially
}

void loop() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Read DHT11 humidity
  float humidity = dht.readHumidity();
  
  // If humidity reading failed, set it to a temporary false value
  if (isnan(humidity)) {
    humidity = -1.0; // Set to -1.0 as a temporary false value
  }

  // Read soil moisture
  int soilMoistureValue = readSensor();

  // Send data if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.0.191:5000/temperature");
    http.addHeader("Content-Type", "application/json");

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Soil Moisture: ");
    Serial.println(soilMoistureValue);

    String jsonData = "{\"temp\":" + String(temperature) + ", \"humidity\":" + String(humidity) + ", \"soil_moisture\":" + String(soilMoistureValue) + "}";
    Serial.println(jsonData);

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(500);
}

// Function to read soil moisture, powering the sensor temporarily
int readSensor() {
  digitalWrite(SOIL_MOISTURE_POWER, HIGH);
  delay(10);
  int value = analogRead(SOIL_MOISTURE_PIN);
  digitalWrite(SOIL_MOISTURE_POWER, LOW);
  return value;
}
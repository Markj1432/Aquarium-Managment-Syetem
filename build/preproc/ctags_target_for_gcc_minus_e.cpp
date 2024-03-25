# 1 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino"
# 2 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino" 2
# 3 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino" 2
# 4 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino" 2
# 5 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino" 2
# 6 "C:\\Projects\\TeaTempExtra\\tempsens\\tempsens.ino" 2

const char* ssid = "Juresh 2.4G";
const char* password = "Liksud9766";






OneWire oneWire(23);
DallasTemperature sensors(&oneWire);
DHT dht(5, DHT11);

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
  pinMode(14, 0x03);
  digitalWrite(14, 0x0); // Keep sensor off initially
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
  digitalWrite(14, 0x1);
  delay(10);
  int value = analogRead(2);
  digitalWrite(14, 0x0);
  return value;
}

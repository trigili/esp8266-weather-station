#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// City identifier
const char* cityID = "CITYID";

// Weather API Settings
const char* apiKey = "APIKEY";
const float lat = 00.0000;
const float lon = 00.0000;

// Wi-Fi credentials
const char *ssid = "WIFISSID";
const char *password = "WIFIPASSWORD";

// MQTT server and credentials
const char *mqttServer = "192.168.10.20";
const int mqttPort = 1883;
const char *mqttUser = "MQTTUSER";
const char *mqttPassword = "MQTTPASSWORD";
const char *mqttTopic = "MQTTTOPIC";

String weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?lat=" + String(lat, 4) + "&lon=" + String(lon, 4) + "&appid=" + apiKey;

// Data structure for weather data
struct WeatherData {
  float temp;
  int humidity;
};

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NTP settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Initialize WiFiClient
WiFiClient espClient;

// Initialize MQTT client using our WiFiClient
PubSubClient client(espClient);

// Function to update display
void updateDisplay(String formattedTime, float localTemp, WeatherData weatherData) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Time: ");
  display.print(formattedTime);

  display.setCursor(0, 16);
  display.print("Local Temp: ");
  display.print(localTemp, 1);

  display.setCursor(0, 32);
  display.print("Your City");

  display.setCursor(0, 44);
  display.print("Temperature: ");
  display.print(weatherData.temp, 1);

  display.setCursor(0, 56);
  display.print("Humidity: ");
  display.print(weatherData.humidity);
  display.print("%");
  
  display.display();
}

/**
 * Initializes the serial interface, connects to WiFi, sets up the OLED display,
 * starts the NTP time client, and connects to the MQTT broker.
 */
void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected");

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  
  // Display WiFi info on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi connected!");
  display.println(WiFi.localIP());
  display.display();

  // Initialize NTP client
  timeClient.begin();
  timeClient.setTimeOffset(-14400);  // Time offset in seconds (-14400 for EDT)

  // Initialize and connect MQTT client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      client.subscribe(mqttTopic);
    } else {
      delay(2000);
    }
  }
}

/**
 * Fetches weather data from OpenWeatherAPI.
 * @return WeatherData structure containing temperature and humidity.
 */
WeatherData fetchWeatherData() {
  WeatherData data;  // Structure to hold temperature and humidity
  HTTPClient http;

  // Prepare the weather API URL
  http.begin(espClient, weatherApiUrl);

  // Make the HTTP request and get response code
  int httpResponseCode = http.GET();
  
  // Check if the GET request was successful
  if (httpResponseCode > 0) {
    // Parse the HTTP response
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    // Get temperature in Celsius from JSON and convert it to Fahrenheit
    float tempC = doc["main"]["temp"].as<float>() - 273.15;
    data.temp = (tempC * 9.0 / 5.0) + 32.0;

    // Get humidity from JSON
    data.humidity = doc["main"]["humidity"];
  } else {
    // Handle failed HTTP GET request
    Serial.println("Failed to fetch weather data.");
    data.temp = -999;
    data.humidity = -999;
  }

  // Close HTTP connection
  http.end();

  return data;
}


/**
 * callback() - MQTT callback function
 * 
 * This function is called when a new message arrives from the MQTT broker.
 * It processes the payload, converts the temperature to Fahrenheit,
 * fetches the current time via NTP, retrieves local weather, and updates the OLED display.
 *
 * @param topic MQTT topic
 * @param payload MQTT payload
 * @param length Length of the payload
 */
void callback(char *topic, byte *payload, unsigned int length) {
  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  float tempC = messageTemp.toFloat();
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  
  // Fetch OpenWeatherAPI data and update display
  WeatherData weatherData = fetchWeatherData();
  updateDisplay(formattedTime, tempF, weatherData);
}

/**
 * loop() - Main loop function
 *
 * Keeps MQTT and NTP clients updated, refreshing the
 * display with each new MQTT payload.
 */
void loop() {
  client.loop();
  timeClient.update();
}

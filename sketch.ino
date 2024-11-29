#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Wokwi-GUEST";         // Replace with your WiFi SSID
const char* password = ""; // Replace with your WiFi Password
// MQTT Broker (HiveMQ)
const char* mqtt_server = "test.mosquitto.org"; 
const char* mqtt_topic = "iotmicro/project";   // MQTT topic 
const float GAMMA = 0.7;
const float RL10 = 50;

// Define your pin connections
#define TFT_CS    15  // Chip select
#define TFT_RST   4   // Reset
#define TFT_DC    2   // Data/command
#define LIGHT_SENSOR_PIN 33 
#define DHTPIN 25
#define DHTTYPE DHT22  

WiFiClient espClient;
PubSubClient client(espClient); // MQTT client

// Initialize the display
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  display.begin();
  display.fillScreen(ILI9341_BLACK); // Clear screen with black
  display.setRotation(0);
  display.setTextSize(2);
  analogReadResolution(10);
  dht.begin();
  setup_WIFI();   
  client.setServer(mqtt_server, 1883);  // Set HiveMQ broker address and port       
  client.setCallback(relay_op);     
}

void loop() {
  if (!client.connected()) {
    reconnect();               // Reconnect if the connection is lost
  }
  client.loop();
  float ph = photores();
  float temp , hum;
  t_h(&temp,&hum);
  // Creating a JSON formatted string to send via MQTT
  String payload = "{\"temperature\": ";
  payload += String(temp);
  payload += ", \"humidity\": ";
  payload += String(hum);
  payload += ", \"light\": ";
  payload += String(ph);
  payload += "}";
  // Publish temperature and humidity data to the MQTT broker
  Serial.print("Publishing message: ");
  Serial.println(payload);

  client.publish(mqtt_topic, payload.c_str());

  delay(5000);
}

float photores() {
  // Read the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  String a;

  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  // Clear the top area for text by drawing a black rectangle over it
  display.fillRect(0, 0, 240, 40, ILI9341_BLACK);
  
  // Display analog value at the top horizontally
  display.setCursor(10, 10);
  display.print("Analog: ");
  display.print(lux);

  // Display brightness level next to the analog value
  display.setCursor(130, 10); // Adjust the position as needed
  if (analogValue < 40) {
    display.print(" Very bright");
    a = "Very bright";
  } else if (analogValue < 800) {
    display.print(" Bright");
    a = "Bright";
  } else if (analogValue < 2000) {
    display.print(" Light");
    a = "Light";
  } else if (analogValue < 3200) {
    display.print(" Dim");
    a = "Dim";
  } else {
    display.print(" dark");
    a = "Dark";
  }

  delay(1000); // Adjust delay as needed
  return lux;
}

void t_h(float *temp , float *hum)
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Clear a section of the screen for the temperature display
  display.fillRect(0, 40, 240, 80, ILI9341_BLACK);

  // Display temperature value
  display.setCursor(10, 50);
  display.setTextColor(ILI9341_WHITE);
  display.print("Temp: ");
  display.print(temperature); 
  display.print(" C");

  display.setCursor(10, 80);
  display.setTextColor(ILI9341_WHITE);
  display.print("Humidity: ");
  display.print(humidity); 
  display.print("%");
  delay(1000);
  *temp = temperature;
  *hum = humidity;
}

void setup_WIFI()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);  // Connect to the WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void relay_op(char* topic, byte* message, unsigned int length) 
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");

  String receivedMessage;

  for (int i = 0; i < length; i++) {
    receivedMessage += (char)message[i];
  }
  Serial.println(receivedMessage);

  // Example: Control a relay based on the received message
  if (receivedMessage == "ON") {
    Serial.println("Turning on the relay...");
    // Add hardware control code here
  } else if (receivedMessage == "OFF") {
    Serial.println("Turning off the relay...");
    // Add hardware control code here
  } else {
    Serial.println("Unknown command received.");
  }

}

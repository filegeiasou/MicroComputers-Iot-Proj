#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "DHT.h"

// Define your pin connections
#define TFT_CS    15  // Chip select
#define TFT_RST   4   // Reset
#define TFT_DC    2   // Data/command
#define LIGHT_SENSOR_PIN 33 // ESP32 pin GIOP33 (ADC0)
#define DHTPIN 25
#define DHTTYPE DHT22  

// Initialize the display
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  display.begin();
  display.setRotation(1); // Adjust rotation if needed
  display.fillScreen(ILI9341_BLACK); // Clear screen with black
  display.setRotation(0);
  analogReadResolution(10);
  dht.begin();
}

void loop() {
  // Set the color to draw the shape
  photores();
  t_h();
  delay(2000);
  /*display.fillRect(50, 50, 100, 100, ILI9341_RED); // Draw a red rectangle
  delay(2000);  // Wait for 2 seconds
  
  display.fillRect(50, 50, 100, 100, ILI9341_BLUE); // Change to blue rectangle
  delay(2000);  // Wait for 2 seconds*/
}

void photores() {
  // Read the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(LIGHT_SENSOR_PIN);

  // Clear the top area for text by drawing a black rectangle over it
  display.fillRect(0, 0, 240, 40, ILI9341_BLACK);
  
  // Display analog value at the top horizontally
  display.setCursor(10, 10);
  display.print("Analog: ");
  display.print(analogValue);

  // Display brightness level next to the analog value
  display.setCursor(130, 10); // Adjust the position as needed
  if (analogValue < 40) {
    display.print(" Very bright");
  } else if (analogValue < 800) {
    display.print(" Bright");
  } else if (analogValue < 2000) {
    display.print(" Light");
  } else if (analogValue < 3200) {
    display.print(" Dim");
  } else {
    display.print(" dark");
  }

  delay(1000); // Adjust delay as needed
}

void t_h()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Clear a section of the screen for the temperature display
  display.fillRect(0, 40, 240, 40, ILI9341_BLACK);

  // Display temperature value
  display.setCursor(10, 50);
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.print("Temp: ");
  display.print(temperature); // Display with 1 decimal place
  display.print(" C");

  display.setCursor(10, 80);
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.print("Humidity: ");
  display.print(humidity); // Display with 1 decimal place
  display.print(" %");
  delay(1000);
}

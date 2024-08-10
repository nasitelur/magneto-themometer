#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int hallSensorPin = A0;

const float sensitivity = 0.0025; // Sensitivity of Sensor in V/G
const float offsetVoltage = 2.55; // Offset Voltage of Sensor
const float voltageConversionFactor = 5.0 / 1023.0;
const float microTeslaConversionFactor = 100.0;
const float yIntercept = 59086.0; // Derived from Linear Regression
const float gradient = 498.65; // Derived from Linear Regression

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    pinMode(hallSensorPin, INPUT);
    Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true);
    }
    display.display();
    delay(2000); // Startup Delay
    display.clearDisplay();
}

void loop() {
    float analogValue = analogRead(hallSensorPin);
    float voltage = analogValue * voltageConversionFactor;

    float magneticFieldGauss = (voltage - offsetVoltage) / sensitivity;
    float magneticFieldMicroTesla = magneticFieldGauss * microTeslaConversionFactor;

    float temperature = calculateTemperature(magneticFieldMicroTesla);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Display Magnetic Field Value
    displayCenteredText("B: " + String(magneticFieldMicroTesla, 2) + " uT", SCREEN_HEIGHT / 2 - 8);

    // Display Temperature Value
    if (temperature >= 0 && temperature <= 95) {
        displayCenteredText("T: " + String(temperature, 2) + " " + (char)248 + "C", SCREEN_HEIGHT / 2 + 10);
    } else {
        displayCenteredText("Out of Range!", SCREEN_HEIGHT / 2 + 10);
    }

    display.display();
    delay(2000);
}

// Convert Magnetic Field to Temperature
float calculateTemperature(float magneticFieldMicroTesla) {
    return abs(yIntercept - magneticFieldMicroTesla) / gradient;
}

// Display Text on The Center
void displayCenteredText(String text, int16_t y) {
    int16_t x1, y1;
    uint16_t width, height;
    display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
    display.setCursor((SCREEN_WIDTH - width) / 2, y);
    display.print(text);
}

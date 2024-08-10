#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int hallSensorPin = A0;

const float sensitivity = 0.0025; // Sensitivity of Sensor in V/G
const float offsetVoltage = 2.55; // Offset Voltage of Sensor
const float voltageConversionFactor = 5.0 / 1023.0;
const float microTeslaConversionFactor = 100.0;
const float yIntercept = 59086.0; // Derived from Linear Regression
const float gradient = 498.65; // Derived from Linear Regression

#define LCD_COLUMNS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(0x27, LCD_COLUMNS, LCD_ROWS);

void setup() {
    pinMode(hallSensorPin, INPUT);

    lcd.begin();
    lcd.backlight();
}

void loop() {
    float analogValue = analogRead(hallSensorPin);
    float voltage = analogValue * voltageConversionFactor;

    float magneticFieldGauss = (voltage - offsetVoltage) / sensitivity;
    float magneticFieldMicroTesla = magneticFieldGauss * microTeslaConversionFactor;

    float temperature = calculateTemperature(magneticFieldMicroTesla);

    displayValues(magneticFieldMicroTesla, temperature);
    delay(2000);
}

// Convert Magnetic Field to Temperature
float calculateTemperature(float magneticFieldMicroTesla) {
    return abs(yIntercept - magneticFieldMicroTesla) / gradient;
}

// Display Magnetic Field and Temperature
void displayValues(float magneticFieldMicroTesla, float temperature) {
    lcd.clear();

    // Display Magnetic Field Value
    lcd.setCursor(0, 0);
    lcd.print("B: ");
    lcd.print(magneticFieldMicroTesla, 2);
    lcd.print(" uT");

    // Display Temperature Value
    lcd.setCursor(0, 1);
    if (temperature >= 0 && temperature <= 95) {
        lcd.print("T: ");
        lcd.print(temperature, 2);
        lcd.print(" ");
        lcd.write((char)223); // Degree Symbol
        lcd.print("C");
    } else {
        lcd.print("Out of Range!");
    }
}

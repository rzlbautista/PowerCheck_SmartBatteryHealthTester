#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int batteryPin = A0;
const int redLED = 8, orangeLED = 9, greenLED = 10;
const float resistorRatio = (10.2 + 5.1) / 5.15;  // Adjusted for real-world resistance

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    pinMode(redLED, OUTPUT);
    pinMode(orangeLED, OUTPUT);
    pinMode(greenLED, OUTPUT);

}

// Function to take multiple readings and average them
float readVoltage() {
    int total = 0;
    int numSamples = 10;

    for (int i = 0; i < numSamples; i++) {
        total += analogRead(batteryPin);
        delay(5);
    }

    float avgSensorValue = total / (float) numSamples;
    return avgSensorValue * (5.0 / 1023.0) * resistorRatio;
}

// Detect battery type based on voltage
String detectBatteryType(float voltage) {
    if (voltage >= 5.2 && voltage <= 6.5) return "4x AAA";
    else if (voltage > 6.6 && voltage <= 9.5) return "9V Battery";
    else if (voltage >= 2.5 && voltage < 4.0) return "Lithium 3V";
    else if (voltage >= 1.3 && voltage < 2.0) return "AA/AAA";
    else return "No Battery";
}

// Get battery percentage based on type
int getBatteryPercentage(float voltage, String batteryType) {
    if (batteryType == "4x AAA") return map(voltage * 100, 5.2 * 100, 6.5 * 100, 0, 100);
    if (batteryType == "9V Battery") return map(voltage * 100, 6.6 * 100, 9.0 * 100, 0, 100);
    if (batteryType == "Lithium 3V") return map(voltage * 100, 2.5 * 100, 3.2 * 100, 0, 100);
    if (batteryType == "AA/AAA") return map(voltage * 100, 1.2 * 100, 1.6 * 100, 0, 100);
    return 0;
}

void loop() {
    delay(300);  // Allow voltage to stabilize before reading
    float voltage = readVoltage();

    Serial.print("Battery Voltage: ");
    Serial.println(voltage, 2);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voltage: ");
    lcd.print(voltage, 2);
    lcd.print("V");

    String batteryType = detectBatteryType(voltage);
    lcd.setCursor(0, 1);
    lcd.print(batteryType);

    if (batteryType != "No Battery") {
        int batteryPercent = getBatteryPercentage(voltage, batteryType);
        if (batteryPercent > 100) batteryPercent = 100;
        if (batteryPercent < 0) batteryPercent = 0;

        lcd.setCursor(0, 1);
        lcd.print("C:");
        lcd.print(batteryPercent);
        lcd.print("% ");

        // LED Indicator Logic (Fixed Overlap)
        if (batteryPercent < 40) {
            lcd.print("Low  ");
            digitalWrite(redLED, HIGH);
            digitalWrite(orangeLED, LOW);
            digitalWrite(greenLED, LOW);
        } else if (batteryPercent >= 40 && batteryPercent < 70) {
            lcd.print("Med  ");
            digitalWrite(redLED, LOW);
            digitalWrite(orangeLED, HIGH);
            digitalWrite(greenLED, LOW);
        } else {
            lcd.print("Good ");
            digitalWrite(redLED, LOW);
            digitalWrite(orangeLED, LOW);
            digitalWrite(greenLED, HIGH);
        }
    } else {
        lcd.setCursor(0, 1);
        lcd.print("No Battery     ");
        digitalWrite(redLED, LOW);
        digitalWrite(orangeLED, LOW);
        digitalWrite(greenLED, LOW);
    }

    delay(2000);
}

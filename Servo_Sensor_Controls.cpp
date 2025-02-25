#include <Servo.h>
#include "U8x8lib.h" // include the fast graphics library
#include "Wire.h"    // include the I2C library
#include <cstring>
U8X8_SSD1306_128X32_UNIVISION_HW_I2C oled(U8X8_PIN_NONE);

const int MAX_REFRESH = 1000;
unsigned long lastClear = 0;


// Define pins
const int sensorPin = A0;  // Moisture sensor input
const int servoPin = 9;    // Servo motor connected to pin 9

Servo valveServo;  // Create servo object

void setupDisplay() {   
    oled.begin(); // Initializes u8x8 object
    oled.setPowerSave(0); // Makes sure OLED doesn't go to sleep
    oled.setFont(u8x8_font_amstrad_cpc_extended_r); // Set the font
    oled.setCursor(0, 0); //Sets the cursor at the top left corner
}

void writeDisplay(const char * message, int row, bool erase) {
    unsigned long now = millis();
    //if erase is true and it's been longer than MAX_REFRESH (1s)
    if(erase && (millis() - lastClear >= MAX_REFRESH)) {
        oled.clearDisplay();
        lastClear = now;
    }
    oled.setCursor(0, row);
    oled.print(message);
}
/*
 * A function to write a CSV message on multiple lines on the OLED.
 * The “commaCount” argument says how many commas are in the String.
 * NOTE: The OLED can only display 4 lines (a maximum of 3 commas).
 */
void writeDisplayCSV(String message, int commaCount) {
     int startIndex = 0;
     for(int i=0; i<=commaCount; i++) {
          // find the index of the comma and store it in startIndex
          int index = message.indexOf(',', startIndex);           

          // take everything in the string up until the comma
          String subMessage = message.substring(startIndex, index); 

          startIndex = index + 1; // skip over the comma

          // Write the substring onto the OLED!
          writeDisplay(subMessage.c_str(), i, false);
     }
}
void setup() {
    Serial.begin(115200);  // Start serial communication
    pinMode(sensorPin, INPUT);
    valveServo.attach(servoPin);
    setupDisplay();
}

void loop() {
    int sensorValue = analogRead(sensorPin); // Read soil moisture (0-1023)
    
    // Convert raw sensor value to percentage (0 = Dry, 100 = Wet)
    int moisturePercentage = map(sensorValue, 0, 1023, 0, 100);
    moisturePercentage = constrain(moisturePercentage, 0, 100); // Ensure within range

    int servoPosition;  // Variable for servo angle

    // Control water flow based on moisture level
    
    // if (moisturePercentage <= 25) {
    //     servoPosition = 90; // Fully Open (Max Water Flow)
    // } else if (moisturePercentage <= 50) {
    //     servoPosition = 90; // 75% Open
    // } else if (moisturePercentage <= 70) {
    //     servoPosition = 180;;  // 50% Open (Moderate Flow)
    // } else if (moisturePercentage <= 85) {
    //     servoPosition = 180;;  // 25% Open (Low Flow)
    // } else {
    //     servoPosition = 180;;   // Fully Closed (No Water Flow)
    // }
    //servoPosition = ((100-moisturePercentage)/ 100) * 180;
    //servoPosition = 180;
    servoPosition = 0.9*moisturePercentage + 90;

    valveServo.write(servoPosition);  // Move servo to calculated position

    // Print values for debugging
    Serial.print("Raw Sensor Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Moisture: ");
    Serial.print(moisturePercentage);
    Serial.print("% | Servo Position: ");
    Serial.println(servoPosition);
    String moisture_str = String(moisturePercentage);
    writeDisplay(strncat(const_cast<char*>(moisture_str.c_str()),"% Moisture",16), 0, true);

    delay(2000);  // Delay to prevent rapid servo movement
}

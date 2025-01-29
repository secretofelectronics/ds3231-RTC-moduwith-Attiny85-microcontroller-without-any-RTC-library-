#include <TinyWireM.h>      // I2C library for ATtiny85
#include <Tiny4kOLED.h>     // Library for SSD1306 OLED

#define DS3231_ADDRESS 0x68

#define BUTTON_HOUR PB1   // Hour Set Button
#define BUTTON_MIN PB3    // Minute Set Button
#define BUTTON_SET PB4    // Set Time Button

byte hour, minute, second;

void setup() {
    TinyWireM.begin();     // Initialize I2C communication

    pinMode(BUTTON_HOUR, INPUT_PULLUP);
    pinMode(BUTTON_MIN, INPUT_PULLUP);
    pinMode(BUTTON_SET, INPUT_PULLUP);

    oled.begin();
    oled.clear();
    oled.on();
    oled.setFont(FONT8X16);  // Set font size
}

void loop() {
    readDS3231Time(hour, minute, second);
    
    if (digitalRead(BUTTON_SET) == LOW) {
        setTime();  // Set time using buttons
    }

    oled.clear();
    oled.setCursor(0, 0);
  
    oled.print("Time: ");
    oled.print(twoDigit(hour));
    oled.print(":");
    oled.print(twoDigit(minute));
    oled.print(":");
    oled.print(twoDigit(second));

    delay(1000);
}

// Function to set time using buttons
void setTime() {
    byte newHour = hour;
    byte newMinute = minute;

    oled.clear();
    oled.setCursor(0, 0);
    oled.print("Set Time:");
    
    delay(500);

    while (digitalRead(BUTTON_SET) == LOW); // Wait for release

    while (digitalRead(BUTTON_SET) == HIGH) {  // Wait until pressed again
        if (digitalRead(BUTTON_HOUR) == LOW) {
            newHour = (newHour + 1) % 24;
            delay(300);
        }

        if (digitalRead(BUTTON_MIN) == LOW) {
            newMinute = (newMinute + 1) % 60;
            delay(300);
        }

        oled.clear();
        oled.setCursor(0, 0);
        oled.print("Set Time:");
        oled.setCursor(0, 2);
        oled.print(twoDigit(newHour));
        oled.print(":");
        oled.print(twoDigit(newMinute));

        delay(100);
    }

    setDS3231Time(newHour, newMinute, 0);  // Set new time
}

// Function to read time from DS3231
void readDS3231Time(byte &hour, byte &minute, byte &second) {
    TinyWireM.beginTransmission(DS3231_ADDRESS);
    TinyWireM.write(0x00);  // Set DS3231 register pointer to 0
    TinyWireM.endTransmission();
    
    TinyWireM.requestFrom(DS3231_ADDRESS, 3);
    second = bcdToDec(TinyWireM.read() & 0x7F);
    minute = bcdToDec(TinyWireM.read());
    hour = bcdToDec(TinyWireM.read() & 0x3F);
}

// Function to set time in DS3231
void setDS3231Time(byte hour, byte minute, byte second) {
    TinyWireM.beginTransmission(DS3231_ADDRESS);
    TinyWireM.write(0x00); // Set register pointer to 0
    TinyWireM.write(decToBcd(second));
    TinyWireM.write(decToBcd(minute));
    TinyWireM.write(decToBcd(hour));
    TinyWireM.endTransmission();
}

// Function to convert BCD to decimal
byte bcdToDec(byte val) {
    return (val / 16 * 10) + (val % 16);
}

// Function to convert decimal to BCD
byte decToBcd(byte val) {
    return (val / 10 * 16) + (val % 10);
}

// Function to format numbers as two digits
String twoDigit(byte num) {
    return (num < 10) ? "0" + String(num) : String(num);
}

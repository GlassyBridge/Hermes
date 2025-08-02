#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BH_ADDR 0x23

#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned long measurementInterval = 120; //ms
unsigned long lastMeasured = 0;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    
    // OLED Initialization
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // loop if failed
    }

    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Luminance Meter");
    display.display();
    delay(1000);
    display.clearDisplay();

    initBH();
}

void loop() {
    float lux = readBH();

    if (lux){ // If lux has value
        display.clearDisplay();
    
        display.setCursor(0, 0); 
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.println("Light Intensity:");
    
        display.setCursor(0, 20);
        display.setTextSize(3);
        display.print(lux, 1);
        display.setTextSize(1);
        display.println(" lux");
    
        display.display();
    
        Serial.print("Light: ");
        Serial.print(lux, 1);
        Serial.println(" lx");
    } else {
        Serial.println("An error occured: couldn't read luminance data.")
    }
}

void initBH() { // BH1750 Initialization
    // Waking up BH1750
    Wire.beginTransmission(BH_ADDR);
    Wire.write(0x01);
    Wire.endTransmission();
    // Reseting BH1750 and turning on Read
    Wire.beginTransmission(BH_ADDR);
    Wire.write(0x07); // Reset
    Wire.write(0x10); // Continous High res reading (1lx precision & 120 ms delay)
    Wire.endTransmission();
    lastMeasured = millis();
}

float readBH() {
    if (millis() - lastMeasured >= measurementInterval) {
        Wire.requestFrom(BH_ADDR);

        unsigned int rawLux = Wire.read();
        rawLux = (rawLux << 8) | Wire.read();

        float lux = rawLux / 1.2;
        lastMeasured = millis();
        return lux;
    }
}
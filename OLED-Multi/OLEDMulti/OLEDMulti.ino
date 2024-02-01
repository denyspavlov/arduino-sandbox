// #include <MemoryUsage.h>
#include "MemProbe.h"

#include <string.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define MUX_ADDR 0x70

// OLED
// PINs A4 (SDA) and A5 (SLA) preconfigured for I2C
#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/*
  Must be only one instance for some reason (array of instances does not work)
  
  One idea was that this was caused due to memory issue and tried the code on Mega2560
  with plenty of spare space (1 instance uses 1K). So there was 6K spare on mega but 
  when second OLED starts up the transfer data is scrambled, which makes me think it is
  a problem with Adafruit_SSD1306 library (potentially some global variables sharing).
 
  However I got it to working with just one instance with a few slightly odd behaviour
  It seems like if the displays are not fully updated data from one device prints on
  the second one. This is why the "swap" function was introduced.

  In conclusion you can have multi-display but you need to refresh full screen after
  swapping the channel, which means that full display state has to be stored on 
  Arduino.
 */
Adafruit_SSD1306 oled0(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_SSD1306 oled1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long timer = 0;
unsigned long lastDiff = 0;

bool swap = 0;
    
void setup() {
  Serial.begin(9600);
  
  i2CdeviceScan();
  
  memCheck(); 
  initOLED(0, &oled0);
  memCheck(); 
  initOLED(1, &oled0);
  memCheck(); 
  i2cMux(0);
  oledClear(&oled0);
  oledPrint(&oled0, 0, 0, "I am 0 and I am up.");  
  i2cUnMux();
  i2cMux(1);
  oledClear(&oled0);
  oledPrint(&oled0, 0, 0, "I am 1 and I am up.");
  i2cUnMux();
  timer = millis();
}

void loop() {


  if (Serial.available()) {
    const String in = Serial.readStringUntil('\n');

    if (in.equals("swap")) {
      swap = swap ^ 1;
    }

    i2cMux(swap ? 1 : 0);
    oledClear(&oled0);
    oledPrint(&oled0, 0, 0, "I am 0 and I am up.");  
    oledPrint(&oled0, 0, 10, "Got input");
    oledPrint(&oled0, 0, 20, in);
    i2cUnMux();
    i2cMux(swap ? 0 : 1);
    oledClear(&oled0);
    oledPrint(&oled0, 0, 0, "I am 1 and I am up.");  
    oledPrint(&oled0, 0, 10, "                  ");
    oledPrint(&oled0, 0, 20, "                  ");
    i2cUnMux();
    
  }

  i2cMux(swap ? 0 : 1);
  printTime(&oled0);
  i2cUnMux();
}

void printTime(Adafruit_SSD1306 * ptr) {
  unsigned long now = millis();
  int diffInSeconds = (int) ((now - timer) / 1000);
  if (diffInSeconds - lastDiff > 0) {
    String timeAsString = String(diffInSeconds);
    oledPrint(ptr, 124 - timeAsString.length() * 6, 50, timeAsString);
    memCheck();
    int freeP = (int) ((((uint32_t) __mem_probe__.mFree) * (uint32_t) 100) / ((uint32_t) __mem_probe__.mDynamic));
    String memAsString = String(__mem_probe__.mFree) + " (" + freeP + "%)";
    oledPrint(ptr, 124 - memAsString.length() * 6, 40, memAsString);
    lastDiff = diffInSeconds;
  }
}

void i2cMux(uint8_t i) {
    if (i > 7) return;
    Wire.beginTransmission(MUX_ADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}

void i2cUnMux() {
    Wire.beginTransmission(MUX_ADDR);
    Wire.write(0);
    Wire.endTransmission();
}


void initOLED(uint8_t i, Adafruit_SSD1306 * ptr) {
  Serial.print("Starting OLED ");
  Serial.println(i);
  i2cMux(i);
  // initialize OLED display with I2C address 0x3C
  if (!ptr->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.print("Failed to start SSD1306 OLED ");
    Serial.println(i);
    delay(100);
  }
  i2cUnMux();
  delay(2000);         // wait two seconds for initializing
  Serial.print("OLED ");
  Serial.print(i);
  Serial.println(" ready");
}

void oledClear(Adafruit_SSD1306 * ptr) {
  ptr->clearDisplay();
  ptr->display();
}

void oledPrint(Adafruit_SSD1306 * ptr, int x, int y, String text) {
  ptr->setTextSize(1);
  ptr->setTextColor(WHITE, BLACK); // black background remove residue text
  ptr->setCursor(x, y);
  ptr->print(text);
  ptr->display();
  Serial.print("Printed: ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(text);
  delay(10);
}

void i2CdeviceScan() {
  Serial.println("Scanning I2C");
  byte count = 0;
  Wire.begin();
  for (uint8_t t=0; t<8; t++) {
    i2cMux(t);
    Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == MUX_ADDR) continue;

      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        count++;
      }
    }
  }

  i2cUnMux();

  Serial.println ("Scan completed");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}

void memCheck() {
  memProbe();
  // Serial.print(F("__mem_probe__.mTotal = "));
  // Serial.println(__mem_probe__.mTotal);
  // Serial.print(F("__mem_probe__.mStaticData = "));
  // Serial.println(__mem_probe__.mStaticData);
  // Serial.print(F("__mem_probe__.mStaticBSS = "));
  // Serial.println(__mem_probe__.mStaticBSS);
  // Serial.print(F("__mem_probe__.mDynamic = "));
  // Serial.println(__mem_probe__.mDynamic);
  // Serial.print(F("__mem_probe__.mHeap = "));
  // Serial.println(__mem_probe__.mHeap);
  // Serial.print(F("__mem_probe__.mStack = "));
  // Serial.println(__mem_probe__.mStack);
  // Serial.print(F("__mem_probe__.mFree = "));
  // Serial.println(__mem_probe__.mFree);

    // MEMORY_PRINT_START
    // MEMORY_PRINT_HEAPSTART
    // MEMORY_PRINT_HEAPEND
    // MEMORY_PRINT_STACKSTART
    // MEMORY_PRINT_END
    // MEMORY_PRINT_HEAPSIZE

    // Serial.println();
    // Serial.println();
   
    // FREERAM_PRINT;
}

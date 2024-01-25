#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
// PINs A4 (SDA) and A5 (SLA) preconfigured for I2C
#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long timer = 0;
unsigned long lastDiff = 0;

void setup() {
  Serial.begin(9600);
  i2CdeviceScan();
  initOLED();
  oledPrint(0, 0, "I am up. Type something and I will echo.");
  timer = millis();
}

void loop() {
  if (Serial.available()) {
    oled.clearDisplay();
    oledPrint(0, 0, "Got input");
    oledPrint(0, 10, Serial.readString());
  }
  printTime();
}

void printTime() {
  unsigned long now = millis();
  unsigned long diffInSeconds = (now - timer) / 1000;
  if (diffInSeconds - lastDiff > 0) {
    String timeAsString = String(diffInSeconds);
    oledPrint(124 - timeAsString.length() * 6, 50, timeAsString);
    lastDiff = diffInSeconds;
  }
}

void initOLED() {
  Serial.println ("Starting OLED");
    // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Failed to start SSD1306 OLED");
    delay(100);
  }
  delay(2000);         // wait two seconds for initializing
  oled.clearDisplay(); // clear display
  Serial.println ("OLED ready");
}

void oledPrint(int x, int y, String text) {
  oled.setTextSize(1);
  oled.setTextColor(WHITE, BLACK); // black background remove residue text
  oled.setCursor(x, y);
  oled.print(text);
  oled.display();
  Serial.print("Printed: ");
  Serial.println(text);
}

void i2CdeviceScan() {
  Serial.println("Scanning I2C");
  byte count = 0;
  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1); 
      } 
  } 
  Serial.println ("Scan completed");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}

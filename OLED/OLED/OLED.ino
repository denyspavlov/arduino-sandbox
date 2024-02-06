#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define I2C 1

// OLED
// PINs A4 (SDA) and A5 (SLA) preconfigured for I2C
#define OLED_I2C_RESET -1 
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// UNO SPI (software SPI)
// #define OLED_MOSI   9
// #define OLED_CLK   10
// #define OLED_DC    11
// #define OLED_CS    12
//#define OLED_RESET 13
// Pro Mini SPI (Hardware SPI)
#define OLED_DC     6
#define OLED_CS     7
#define OLED_SPI_RESET  8

// IIC
#ifdef I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_I2C_RESET);
#else
// SPI UNO (software)
// Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT,
//   OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// SPI Pro Mini (hardware)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_SPI_RESET, OLED_CS);
#endif

unsigned long timer = 0;
unsigned long lastDiff = 0;

void setup() {
  Serial.begin(9600);
  
  //Wire.begin();
  //SPI.begin();

  //i2CdeviceScan();
  //spiScan();
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
  int diffInSeconds = (int) ((now - timer) / 1000);
  if (diffInSeconds - lastDiff > 0) {
    String timeAsString = String(diffInSeconds);
    oledPrint(124 - timeAsString.length() * 6, 50, timeAsString);
    lastDiff = diffInSeconds;
  }
}

void initOLED() {
  Serial.println ("Starting OLED");
  // initialize OLED display with I2C address 0x3C
  #ifdef I2C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Failed to start SSD1306 OLED");
    delay(100);
  }
  #else
  if (!oled.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("Failed to start SSD1306 OLED");
    delay(100);
  }
  #endif
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
  // Wire.begin();
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

void spiScan() {
  Serial.println("Scanning for SPI devices...");
  // SPI.begin();
  // Loop through all possible SS pins (10-15 on Uno, Mega, etc.)
  for (int ssPin = 2; ssPin <= 9; ssPin++) {
    pinMode(ssPin, OUTPUT);
    digitalWrite(ssPin, HIGH); // Deselect all slaves before selecting one
    
    // Send a dummy byte to the current slave select pin
    SPI.setDataMode(SPI_MODE0); // Set SPI mode to default mode 0
    SPI.setClockDivider(SPI_CLOCK_DIV4); // Slow down SPI clock speed to avoid interference
    uint8_t dummyByte = SPI.transfer(0xFF);
    
    // Check if the received byte is different from the sent byte
    if (dummyByte != 0xFF) {
      Serial.print("Found SPI device at SS pin ");
      Serial.println(ssPin);
    } else {
      // If no response, print a dot to indicate progress
      Serial.print(".");
    }
    
    digitalWrite(ssPin, HIGH); // Ensure SS pin is high after scanning
    pinMode(ssPin, INPUT); // Reset pin mode to prevent noise
  }
  
  Serial.println();
}

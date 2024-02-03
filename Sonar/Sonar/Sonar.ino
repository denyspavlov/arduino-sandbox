#include <Arduino.h>
#include <Servo.h>
#include <math.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "MemProbe.h"
#include "UltraSonic.h"
#include "ToggleButton.h"
#include "MultiToggleButton.h"

#define SONIC_TRIGGER 9
#define SONIC_ECHO 8

#define SERVO_CTRL 7

#define ON_BTN 10
#define ZOOM_BTN 11

// Maximum is 400cm, but for a small OLED it is better to limit range
#define SONAR_RANGE 400

// OLED
// PINs A4 (SDA) and A5 (SLA) preconfigured for I2C
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

UltraSonic sonic(SONIC_TRIGGER, SONIC_ECHO);
Servo pos;
ToggleButton sonarButton(ON_BTN, LOW, 50);
MultiToggleButton zoomButton(ZOOM_BTN, 3, LOW, 50);

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int zoomLevel = SONAR_RANGE;
byte sonarAngle = 90;
bool sonarSweepToZero = true;
float sonarSweepData[180];
byte sonarMap[180][2];


void initOLED() {
  Serial.println(F("Starting OLED"));
  // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Failed to start SSD1306 OLED"));
    delay(100);
  }
  delay(2000);  // wait two seconds for initializing
  oledClear();
  Serial.println(F("OLED ready"));
}

void oledWelcome() {
  oled.clearDisplay();
  oledSweepScreenZoom();
  oledSweepScreenAngle();
  oledPrint(0, 20, "Sonar is ready, press sonar button to start sweep, or use zoom button to select the range");
  oled.display();
}

void oledClear() {
  oled.clearDisplay();  // clear display
  oled.display();
}

void oledSweepScreen() {
  oledClear();
  oledSweepScreenZoom();
  oledSweepScreenAngle();
}

void oledSweepScreenZoom() {
  oledPrint(0, 0, "R:");
  oledPrint(12, 0, String(zoomLevel));
  oledPrint(SCREEN_WIDTH - 30, 0, "A:");
}

void oledSweepScreenAngle() {
  if (sonarAngle < 10) {
    oledPrint(SCREEN_WIDTH - 18, 0, "  ");
    oledPrint(SCREEN_WIDTH - 8, 0, String(sonarAngle));
  } else if (sonarAngle < 100) {
    oledPrint(SCREEN_WIDTH - 18, 0, " ");
    oledPrint(SCREEN_WIDTH - 12, 0, String(sonarAngle));
  } else {
    oledPrint(SCREEN_WIDTH - 18, 0, String(sonarAngle));
  }
}

void oledPrintPixel(int x, int y, bool black = false) {
  oled.drawPixel(x, y, black ? BLACK : WHITE);
  oled.display();
}

void oledPrint(int x, int y, String text) {
  oled.setTextSize(1);
  oled.setTextColor(WHITE, BLACK);  // black background remove residue text
  oled.setCursor(x, y);
  oled.print(text);
  oled.display();
}


void resetSweep() {
  // re-align to start position
  pos.write(90);
  // adjust data points
  sonarAngle = 90;
  sonarSweepToZero = true;
  for (int i = 0; i < 180; i++) {
    sonarSweepData[i] = 500;  // assume there is an obstacle within 5m radius
    sonarMap[i][0] = 0;       // x
    sonarMap[i][1] = 0;       // y
  }
}

void onSonarButtonPress(bool toggle) {
  if (!toggle) {
    resetSweep();
    oledWelcome();
  } else {
    oledSweepScreen();
  }
}

void onZoomButtonPress(int toggle) {
  switch (toggle) {
    case 0:
      zoomLevel = SONAR_RANGE;
      break;
    case 1:
      zoomLevel = SONAR_RANGE / 2;
      break;
    case 2:
      zoomLevel = SONAR_RANGE / 4;
      break;
  }
  oledSweepScreenZoom();
}

/*
 * Sonar view
 *          90*
 *           |
 * 180*  ____|____ 0*
 * 
 * OLED 124x64,
 * mid point x,y = 62,60 
 * 90* x,y = 62,0
 * 180* x,y = 2,60
 * 0* = 120,60
 */
void plotXY(byte angle, float distance) {

  if (distance < 2) {
    // too close do not plot
    return;
  }

  byte lastX = sonarMap[sonarAngle][0];
  byte lastY = sonarMap[sonarAngle][1];

  float radians = ((float)angle) * PI / 180.0f;
  int x = (int)(distance * cos(radians));
  int y = (int)(distance * sin(radians));

  sonarMap[sonarAngle][0] = (byte)map(x, -zoomLevel, zoomLevel, 2, SCREEN_WIDTH - 2);
  sonarMap[sonarAngle][1] = (byte)map(y, 0, zoomLevel, SCREEN_HEIGHT - 2, 2);

  Serial.print(F("plotXY "));
  Serial.print(sonarAngle);
  Serial.print(F("*, "));
  Serial.print(sonarSweepData[sonarAngle]);
  Serial.print(F("cm, (x,y)->("));
  Serial.print(x);
  Serial.print(F(","));
  Serial.print(y);
  Serial.print(F(")->("));
  Serial.print(sonarMap[sonarAngle][0]);
  Serial.print(F(","));
  Serial.print(sonarMap[sonarAngle][1]);
  Serial.println(F(")"));

  oledPrintPixel(lastX, lastY, true);
  oledPrintPixel(sonarMap[sonarAngle][0], sonarMap[sonarAngle][1], false);
  oledSweepScreenAngle();
}

void sweepOnce(bool changePosition = true) {

  sonarSweepData[sonarAngle] = sonic.getDistanceCm();
  plotXY(sonarAngle, sonarSweepData[sonarAngle]);

  Serial.print(F("sweeping "));
  Serial.print(sonarAngle);
  Serial.print(F("*, "));
  Serial.print(sonarSweepData[sonarAngle]);
  Serial.print(F("cm, (x,y)->("));
  Serial.print(sonarMap[sonarAngle][0]);
  Serial.print(F(","));
  Serial.print(sonarMap[sonarAngle][1]);
  Serial.println(F(")"));

  if (changePosition) {
    if (sonarSweepToZero) {
      if (sonarAngle > 0) {
        sonarAngle--;  // move 1 degree towards zero
        pos.write(sonarAngle);
      } else {
        sonarSweepToZero = false;  // change the direction
      }
    } else {
      if (sonarAngle < 180) {
        sonarAngle++;  // move 1 degree towards one
        pos.write(sonarAngle);
      } else {
        sonarSweepToZero = true;  // change the direction
      }
    }

    delay(10);  // ensure we moved
  }
}

void setup() {
  Serial.begin(9600);
  memCheck();
  sonic.attach();
  pos.attach(SERVO_CTRL);
  sonarButton.attach(onSonarButtonPress);
  zoomButton.attach(onZoomButtonPress);
  initOLED();
  resetSweep();
  oledWelcome();
}


void loop() {

  if (sonarButton.isToggleOn()) {
    sweepOnce();
  } else {
    zoomButton.getToggle();  // read the toggle only when not sweeping
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd.equals("c")) {
      oledSweepScreen();
    } else if (cmd.equals("s")) {
      sweepOnce(false);
      // resetSweep();
    } else {
      int degrees = atoi(cmd.c_str());
      pos.write(degrees);
      sonarAngle = degrees;
      sweepOnce();
      //resetSweep();
    }
  }

}

void memCheck() {

  memProbe();

  Serial.print(F("__mem_probe__.mTotal = "));
  Serial.println(__mem_probe__.mTotal);
  Serial.print(F("__mem_probe__.mStaticData = "));
  Serial.println(__mem_probe__.mStaticData);
  Serial.print(F("__mem_probe__.mStaticBSS = "));
  Serial.println(__mem_probe__.mStaticBSS);
  Serial.print(F("__mem_probe__.mDynamic = "));
  Serial.println(__mem_probe__.mDynamic);
  Serial.print(F("__mem_probe__.mHeap = "));
  Serial.println(__mem_probe__.mHeap);
  Serial.print(F("__mem_probe__.mStack = "));
  Serial.println(__mem_probe__.mStack);
  Serial.print(F("__mem_probe__.mFree = "));
  Serial.println(__mem_probe__.mFree);
}

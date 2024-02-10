#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "MultiToggleButton.h"
#include "Joystick.h"


#define INPUT_SENSITIVITY 100

#define J_RANGE_X 21
#define J_RANGE_Y 4

#define BTN_L1_PIN 5
#define BTN_L2_PIN 4
#define BTN_R1_PIN 3
#define BTN_R2_PIN 2

MultiToggleButton bl1(BTN_L1_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton bl2(BTN_L2_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton br1(BTN_R1_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton br2(BTN_R2_PIN, 1, HIGH, INPUT_SENSITIVITY);

#define J_BTN_PIN 9
#define J_Y_PIN A1
#define J_X_PIN A2

Joystick jst(J_BTN_PIN, J_X_PIN, J_Y_PIN, INPUT_SENSITIVITY, J_RANGE_X, J_RANGE_Y);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_DC     6
#define OLED_CS     7
#define OLED_SPI_RESET  8

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_SPI_RESET, OLED_CS);

int cursorX = 0;
int cursorY = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(F("setup - start"));

  bl1.attach(onBL1);
  bl2.attach(onBL2);
  br1.attach(onBR1);
  br2.attach(onBR2);

  jst.attach(onJSelect, onJXY);

  if (!oled.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("Failed to start SSD1306 OLED");
    delay(100);
  }
  delay(2000);
  oled.clearDisplay();
  oled.display();

  oledPrint(0, 0, "Device is ready", false);

  oledPrintInput(2);

  Serial.println(F("setup - finish"));
}

void loop() {
  bl1.listen();
  bl2.listen();
  br1.listen();
  br2.listen();
  jst.listen();
}


void oledPrint(int col, int row, String text, bool inverse) {
  oled.setTextSize(1);
  oled.setTextColor(inverse ? BLACK : WHITE, inverse ? WHITE : BLACK); // black background remove residue text
  oled.setCursor(col * 6, row * 10);
  oled.print(text);
  oled.display();
  Serial.print("Printed: ");
  Serial.println(text);
}

void oledPrintInput(int startRow) {
  char *inputMap[3][21] = {
    {
      "ABCDEFGHIJKLMNOPQRSTU"
    },
    {
      "WXYZ1234567890!@#$%^&"
    },
    {
      "*()-+=:_;\".|\\</>,.~ "
    }
  };
  int y,x;
  oled.setTextSize(1);
  int startY = startRow * 10;
  int selectX = cursorX;
  int selectY = cursorY + startRow;
  for (y = 0; y < 3; y++) {
    int yRow = startY + y * 10;
    for (x = 0; x < 21; x++) {
      oled.setCursor(x * 6, yRow);
      if (y == selectY && x == selectX) {
        oled.setTextColor(BLACK, WHITE);
      } else {
        oled.setTextColor(WHITE, BLACK);
      }
      oled.print(inputMap[y][x]);
    }
  }
  oled.display();
}

void onBL1(int toggle) {
  Serial.print(F("BL1="));
  Serial.println(toggle);
}

void onBL2(int toggle) {
  Serial.print(F("BL2="));
  Serial.println(toggle);
}

void onBR1(int toggle) {
  Serial.print(F("BR1="));
  Serial.println(toggle);
}

void onBR2(int toggle) {
  Serial.print(F("BR2="));
  Serial.println(toggle);
}

void onJSelect(int toggle) {
  Serial.print(F("JSelect="));
  Serial.println(toggle);
}

void onJXY(int x, int y) {
  Serial.print(F("JXY="));
  Serial.print(x);
  Serial.print(F(","));
  Serial.println(y);
  cursorX = x;
  cursorY = y;
  oledPrintInput(2);
}
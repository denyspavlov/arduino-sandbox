#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

// debug
#define LOG_LEVEL 3
#include "Log.h"

// custom components
#include "Display.h"
#include "Keyboard.h"
#include "PushButton.h"
#include "PushButtonArray.h"

#define INPUT_SENSITIVITY 100

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define OLED_SPI_CLK    18
#define OLED_SPI_MOSI   19
#define OLED_SPI_RESET   5
#define OLED_DC         26
#define OLED_CS         33


Display display(
  OLED_SPI_CLK, OLED_SPI_MOSI, OLED_SPI_RESET, OLED_DC, OLED_CS, 
  SCREEN_WIDTH, SCREEN_HEIGHT
);

#define BTN_PIN 34
#define BTN_MODE 0
#define BTN_DEL 1
#define BTN_SEL 2
#define BTN_CLR 3

PushButtonArray btns(
  BTN_PIN, 4, new int[8] { 1100, 1500, 1700, 2000, 600, 900, 0, 200 }, HIGH, INPUT_SENSITIVITY
); 

#define PTN_PIN 35

Keyboard kbd(
  &display,
  PTN_PIN, 4094, 0,
  BTN_PIN, 1100, 1500, 1700, 2000, 600, 900, 0, 200,
  HIGH, INPUT_SENSITIVITY
);


// OLEDKeyboardSH1107 kbd(
//   OLED_SPI_CLK, OLED_SPI_MOSI, OLED_SPI_RESET, OLED_DC, OLED_CS, SCREEN_WIDTH, SCREEN_HEIGHT,
//   PTN_PIN, 4094, 0,
//   BTN_PIN, 1100, 1500, 1700, 2000, 600, 900, 0, 200,
//   HIGH, INPUT_SENSITIVITY
// );

#define KBD_LAYOUT "ABCDEFGHJIKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-+_=:;\"|\/<>{}[],.               "
#define KBD_LAYOUT_COLS 21
#define KBD_LAYOUT_ROWS 5


#define I2C_ADDR 0x60
#define I2C_DEVICE "OLED_KEYBOARD v0.01"

bool i2cAttached = 0;

bool inputMode = false;

void onInputModeListener() {
  __logDebug__("input mode ON");
}

void onCaptureListener(char* captured, int len) {

  __logDebug__("onCaptureListener=", captured, ",size=", len);

  display.printText(0, 0, F("Your input was:"), false);
  display.printText(0, 1, captured, false);
  display.update();

  inputMode = false;

}

void onButtonPushListener(int btn) {

  __logDebug__("onButtonPushListener=", btn);

  if (btn == BTN_MODE) {
    if (!inputMode) {
      inputMode = true;
      kbd.readInput("Testing Keyboard.\nType something.", KBD_LAYOUT, KBD_LAYOUT_COLS, KBD_LAYOUT_ROWS);
    }
  }

}

void setup() {
  Serial.begin(115200);

  __logDebug__("setup - start");

  display.attach();
  display.printText(0, 0, F("Starting..."), false);
  display.update();

  btns.attach(onButtonPushListener);

  kbd.attach(onInputModeListener, onCaptureListener);

  display.update(true);

  __logDebug__("setup - finish");
}

void loop() {
  btns.listen();

  if (inputMode) {
    kbd.listen();
  }

  // __logDebug__("PTN:", analogRead(PTN_PIN));
  // __logDebug__("BTN:", analogRead(BTN_PIN));
  // delay(300);
}


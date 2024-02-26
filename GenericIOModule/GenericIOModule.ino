#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

// debug
#define LOG_LEVEL 3
#include "Log.h"

#include "OLEDKeyboardSH1107.h"

#define INPUT_SENSITIVITY 100


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define OLED_SPI_CLK    18
#define OLED_SPI_MOSI   19
#define OLED_SPI_RESET   5
#define OLED_DC         26
#define OLED_CS         33

#define PTN_PIN 35
#define BTN_PIN 34

PushButton modeBtn(BTN_PIN, 1100, 1500);

OLEDKeyboardSH1107 kbd(
  OLED_SPI_CLK, OLED_SPI_MOSI, OLED_SPI_RESET, OLED_DC, OLED_CS, SCREEN_WIDTH, SCREEN_HEIGHT,
  PTN_PIN, 4094, 0,
  BTN_PIN, 1100, 1500, 1700, 2000, 600, 900, 0, 200,
  HIGH, INPUT_SENSITIVITY
);

byte inputMap[105] = {
    65 /*'A'*/, 66 /*'B'*/, 67 /*'C'*/, 68 /*'D'*/, 69 /*'E'*/, 
    70 /*'F'*/, 71 /*'G'*/, 72 /*'H'*/, 73 /*'I'*/, 74 /*'J'*/, 
    75 /*'K'*/, 76 /*'L'*/, 77 /*'M'*/, 78 /*'N'*/, 79 /*'O'*/,
    80 /*'P'*/, 81 /*'Q'*/, 82 /*'R'*/, 83 /*'S'*/, 84 /*'T'*/, 
    85 /*'U'*/,
    86 /*'V'*/, 87 /*'W'*/, 88 /*'X'*/, 89 /*'Y'*/, 90 /*'Z'*/, 
    97 /*'a'*/, 98 /*'b'*/, 99 /*'c'*/, 100 /*'d'*/, 101 /*'e'*/, 
    102 /*'f'*/, 103 /*'g'*/, 104 /*'h'*/, 105 /*'i'*/, 106 /*'j'*/, 
    107 /*'k'*/, 108 /*'l'*/, 109 /*'m'*/, 110 /*'n'*/, 111 /*'o'*/,
    112 /*'p'*/,
    113 /*'q'*/, 114 /*'r'*/, 115 /*'s'*/, 116 /*'t'*/, 117 /*'u'*/,
    118 /*'v'*/, 119 /*'w'*/, 120 /*'x'*/, 121 /*'y'*/, 122 /*'z'*/, 
    49 /*'1'*/, 50 /*'2'*/, 51 /*'3'*/, 52 /*'4'*/, 53 /*'5'*/, 
    54 /*'6'*/, 55 /*'7'*/, 56 /*'8'*/, 57 /*'9'*/, 48 /*'0'*/, 
    33 /*'!'*/,
    64 /* '@' */, 35 /* '#' */, 36 /* '$' */, 37 /* '%' */, 94 /* '^' */, 
    38 /* '&' */, 42 /*'*'*/, 40 /*'('*/, 41 /*')'*/, 45 /* '-' */, 
    43 /* '+' */, 95 /* '_' */, 61 /* '=' */, 58 /* ':' */, 59 /* ';' */, 
    34 /* '"' */, 124 /* '|' */, 92 /* '\' */, 47 /* '/' */, 60 /* '<' */,
    62 /* '>' */,
    123 /* '{' */, 125 /* '}' */, 91 /* '[' */, 93 /* ']' */, 44 /* ',' */, 
    46 /* '.' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */
};

#define I2C_ADDR 0x60
#define I2C_DEVICE "OLED_KEYBOARD v0.01"

bool i2cAttached = 0;

void onInputModeListener() {
  __logDebug__("input mode ON");
}

void onCaptureListener(byte* captured, int len) {

  char cap[len + 1];
  int i = 0;
  for (i = 0; i < len; i++) {
    cap[i] = (char) captured[i];
  } 
  cap[i + 1] = '\0';
  __logDebug__("onCaptureListener=", cap, ",size=", len);
}

void setup() {
  Serial.begin(115200);

  __logDebug__("setup - start");

  kbd.attach(onInputModeListener, onCaptureListener);
  // kbd.readInput(&inputMap[0], 21, 5);
  kbd.readInput("Test all inputs.\nType something.", &inputMap[0], 11, 10);

  __logDebug__("setup - finish");
}

void loop() {
  kbd.listen();
  //__logDebug__("PTN:", analogRead(PTN_PIN));
  //__logDebug__("BTN:", analogRead(BTN_PIN));
  //delay(300);
}


# OLED Keyboard

OLED Keyboard is an input device to capture arbitrary input from an input map which is
displayed on OLED. Joystick is used as a "mouse" to navigate to desired character, which
can then be selected using buttons. There are four buttons: BL1 which allows to go into
input mode and confirm input, BL2 to clear current input while in the input mode, BR1 to
select character currently selected via Joystick, BR2 to remove last character (backspace)

This device requires Arduino Mega since anything useful with OLED component needs more 
than 2K RAM (at least 3K)

*Wokwi*
I2C version only
https://wokwi.com/projects/389359404208663553


*Components*
Arduino MEGA2560 Pro Embed ATmega2560 16AU Compatible Board CH340G 
https://www.ebay.co.uk/itm/185956560929
0.96" OLED 128X64 I2C White Display Arduino SSD1306
https://www.ebay.co.uk/itm/263574014445?var=562623326218
Joystick
4x push buttons
4x 10K pull up resistors

```
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// debug
#define LOG_LEVEL 3
#include "Log.h"

#include "OLEDKeyboardSSD1306.h"

#define INPUT_SENSITIVITY 100


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SPI_RESET  13
#define OLED_DC     11
#define OLED_CS     9

#define PTN_PIN A5

#define BTN_MODE_PIN 5
#define BTN_CLR_PIN 4
#define BTN_SEL_PIN 3
#define BTN_DEL_PIN 2

PushButton modeBtn(BTN_MODE_PIN);

OLEDKeyboardSSD1306 kbd(
  OLED_SPI_RESET, OLED_DC, OLED_CS, SCREEN_WIDTH, SCREEN_HEIGHT,
  PTN_PIN, 0, 1023,
  BTN_MODE_PIN, BTN_CLR_PIN, BTN_SEL_PIN, BTN_DEL_PIN,
  HIGH, INPUT_SENSITIVITY
);

const byte inputMap[105] = {
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

byte * onInputModeListener() {
  return &inputMap[0];
}

void onCaptureListener(byte * captured) {

  int len = sizeof(captured) / sizeof(byte); 
  char cap[len];
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
  kbd.readInput(&inputMap[0]);

  __logDebug__("setup - finish");
}

void loop() {
  kbd.listen();
}

```


 
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "MultiToggleButton.h"
#include "Joystick.h"
#include "RangeSelector.h"

// Uncomment = joystick mode, comment = potentiometer mode
//#define JOYSTICK_MODE 1

#define INPUT_SENSITIVITY 100

#define J_RANGE_X 21
#define J_RANGE_Y 5

#define BTN_L1_PIN 5
#define BTN_L2_PIN 4
#define BTN_R1_PIN 3
#define BTN_R2_PIN 2

MultiToggleButton bl1(BTN_L1_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton bl2(BTN_L2_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton br1(BTN_R1_PIN, 1, HIGH, INPUT_SENSITIVITY);
MultiToggleButton br2(BTN_R2_PIN, 1, HIGH, INPUT_SENSITIVITY);

#ifdef JOYSTICK_MODE

#define J_BTN_PIN 7
#define J_Y_PIN A1
#define J_X_PIN A3

Joystick jst(J_BTN_PIN, J_X_PIN, J_Y_PIN, INPUT_SENSITIVITY, J_RANGE_X, J_RANGE_Y);
#else 

#define PTN_PIN A5
RangeSelector rsl(PTN_PIN, INPUT_SENSITIVITY, J_RANGE_X, J_RANGE_Y);

#endif 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SPI_RESET  13
#define OLED_DC     11
#define OLED_CS     9

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_SPI_RESET, OLED_CS);

int cursorX = 10;
int cursorY = 2;

const byte inputMap[5][21] = {
  {
    65 /*'A'*/, 66 /*'B'*/, 67 /*'C'*/, 68 /*'D'*/, 69 /*'E'*/, 
    70 /*'F'*/, 71 /*'G'*/, 72 /*'H'*/, 73 /*'I'*/, 74 /*'J'*/, 
    75 /*'K'*/, 76 /*'L'*/, 77 /*'M'*/, 78 /*'N'*/, 79 /*'O'*/,
    80 /*'P'*/, 81 /*'Q'*/, 82 /*'R'*/, 83 /*'S'*/, 84 /*'T'*/, 
    85 /*'U'*/
  },
  {
    86 /*'V'*/, 87 /*'W'*/, 88 /*'X'*/, 89 /*'Y'*/, 90 /*'Z'*/, 
    97 /*'a'*/, 98 /*'b'*/, 99 /*'c'*/, 100 /*'d'*/, 101 /*'e'*/, 
    102 /*'f'*/, 103 /*'g'*/, 104 /*'h'*/, 105 /*'i'*/, 106 /*'j'*/, 
    107 /*'k'*/, 108 /*'l'*/, 109 /*'m'*/, 110 /*'n'*/, 111 /*'o'*/,
    112 /*'p'*/
  },
  {
    113 /*'q'*/, 114 /*'r'*/, 115 /*'s'*/, 116 /*'t'*/, 117 /*'u'*/,
    118 /*'v'*/, 119 /*'w'*/, 120 /*'x'*/, 121 /*'y'*/, 122 /*'z'*/, 
    49 /*'1'*/, 50 /*'2'*/, 51 /*'3'*/, 52 /*'4'*/, 53 /*'5'*/, 
    54 /*'6'*/, 55 /*'7'*/, 56 /*'8'*/, 57 /*'9'*/, 48 /*'0'*/, 
    33 /*'!'*/
  },
  {
    64 /* '@' */, 35 /* '#' */, 36 /* '$' */, 37 /* '%' */, 94 /* '^' */, 
    38 /* '&' */, 42 /*'*'*/, 40 /*'('*/, 41 /*')'*/, 45 /* '-' */, 
    43 /* '+' */, 95 /* '_' */, 61 /* '=' */, 58 /* ':' */, 59 /* ';' */, 
    34 /* '"' */, 124 /* '|' */, 92 /* '\' */, 47 /* '/' */, 60 /* '<' */,
    62 /* '>' */
  }, 
  {
    123 /* '{' */, 125 /* '}' */, 91 /* '[' */, 93 /* ']' */, 44 /* ',' */, 
    46 /* '.' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */, 32 /* ' ' */,
    32 /* ' ' */
  }
};

bool inputToggle = 1;

#define MAX_INPUT 20

int inputLength = 0;
byte input[MAX_INPUT + 1] = { '\0' };
bool inputConfirmed = 0;

#define I2C_ADDR 0x60
#define I2C_DEVICE "OLED_KEYBOARD v0.01"

bool i2cAttached = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(F("setup - start"));

  bl1.attach(onBL1);
  bl2.attach(onBL2);
  br1.attach(onBR1);
  br2.attach(onBR2);

  #ifdef JOYSTICK_MODE
  jst.attach(onJSelect, onJXY);
  #else 
  rsl.attach(onJXY);
  #endif

  if (!oled.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("Failed to start SSD1306 OLED");
    delay(100);
  }
  delay(2000);

  oledPrintWelcome();

  Serial.println(F("setup - finish"));
}

void loop() {
  br2.listen();
  br1.listen();
  bl2.listen();
  bl1.listen();
  #ifdef JOYSTICK_MODE
  jst.listen();
  #else 
  rsl.listen();
  #endif
}


void oledPrint(int col, int row, String text, bool inverse) {
  oled.setTextSize(1);
  oled.setTextColor(inverse ? BLACK : WHITE, inverse ? WHITE : BLACK); // black background remove residue text
  oled.setCursor(col * 6, row * 10);
  oled.print(text);
  Serial.print("Printed: ");
  Serial.println(text);
}

void oledPrintWelcome() {
  if (!i2cAttached) {
    oled.clearDisplay();
    oled.display();

    oledPrint(0, 0, I2C_DEVICE, false);
    oledPrint(0, 1, "BL1: input mode", false);
    oledPrint(0, 2, "BL2: clear", false);
    oledPrint(0, 3, "Joystick: navigate", false);
    oledPrint(0, 4, "BR1: select", false);
    oledPrint(0, 5, "BR2: delete", false);
    oled.display();

  } else {
    oled.clearDisplay();
    oled.display();
  }
}

void oledPrintInput(int selectX, int selectY) {
  Serial.println(F("Input Panel - start"));
  int y,x;
  oled.setTextSize(1);

  oled.setCursor(0, 0);
  oled.setTextColor(WHITE, BLACK);
  oled.print('>');

  int startY = 10;

  Serial.print(F("Input Panel - select: "));
  Serial.print(selectX);
  Serial.print(F(","));
  Serial.println(selectY);

  for (y = 0; y < 5; y++) {
    int yRow = startY + y * 10;
    Serial.print(y);
    Serial.print(F(": "));
    for (x = 0; x < 21; x++) {
      Serial.print(F("["));
      Serial.print(x);
      Serial.print(F("]"));
      char c = (char) inputMap[y][x];
      oled.setCursor(x * 6, yRow);
      if (y == selectY && x == selectX) {
        oled.setTextColor(BLACK, WHITE);
        Serial.write('<');
        Serial.write(c);
        Serial.write('>');
      } else {
        oled.setTextColor(WHITE, BLACK);
        Serial.write(' ');
        Serial.write(c);
        Serial.write(' ');
      }
      oled.print(c);
    }
    Serial.println();
  }
  oled.display();

  Serial.println(F("Input Panel - end"));
}


void oledPrintInputChangeCursor(int oldX, int oldY, int selectX, int selectY) {
  Serial.println(F("Input Panel change cursor - start"));

  oled.setTextSize(1);

  int startY = 10;

  Serial.print(F("Input Panel - select: "));
  Serial.print(selectX);
  Serial.print(F(","));
  Serial.println(selectY);

  char oldC = (char) inputMap[oldY][oldX];
  char newC = (char) inputMap[selectY][selectX];

  oled.setCursor(oldX * 6, startY + oldY * 10);
  oled.setTextColor(WHITE, BLACK);
  oled.print(oldC);
  oled.setCursor(selectX * 6, startY + selectY * 10);
  oled.setTextColor(BLACK, WHITE);
  oled.print(newC);
  oled.display();
  
  Serial.println(F("Input Panel change cursor - end"));
}

void oledPrintInputValue() {
  Serial.println(F("Input Panel value - start"));

  oled.setTextSize(1);

  oled.setCursor(6, 0);
  oled.setTextColor(WHITE, BLACK);
  oled.print("                    ");
  oled.setCursor(6, 0);
  for (int i = 0; i < inputLength; i++) {
    char c = (char) input[i];
    oled.write(c);
  }  
  oled.display();

  Serial.println(F("Input Panel value - end"));
}


void onBL1(int toggle) {
  Serial.print(F("BL1="));
  Serial.print(toggle);
  Serial.print(F(", inputToggle="));
  Serial.println(inputToggle);

  inputToggle = toggle == 1;
  if (inputToggle == 0) {
    input[0] = '\0';
    inputLength = 0;
    oled.clearDisplay();
    oledPrintInput(cursorX, cursorY);
  } else {
    oledPrintWelcome();
    inputConfirmed = inputLength > 0;
  }
}

void onBL2(int toggle) {
  Serial.print(F("BL2="));
  Serial.print(toggle);
  Serial.print(F(", inputToggle="));
  Serial.println(inputToggle);

  if (inputToggle == 0) {
    input[0] = '\0';
    inputLength = 0;
    oled.clearDisplay();
    oledPrintInput(cursorX, cursorY);
  }
}

void onBR1(int toggle) {
  Serial.print(F("BR1="));
  Serial.print(toggle);
  Serial.print(F(", inputToggle="));
  Serial.println(inputToggle);

  if (inputToggle == 0) {

    byte b = inputMap[cursorY][cursorX];
    Serial.print(F(",v="));
    Serial.write((char) b);
    Serial.println();

    if (inputLength == MAX_INPUT) {
      input[inputLength - 1] = b;
    } else {
      input[inputLength] = b;
      inputLength++;
      input[inputLength] = '\0';
    }
    Serial.print(F("inputLength="));
    Serial.print(inputLength);
    Serial.print(F(",input="));
    for (int i = 0; i < inputLength; i++) {
      char c = (char) input[i];
      Serial.write(c);
    }  


    oledPrintInputValue();
  }
}

void onBR2(int toggle) {
  Serial.print(F("BR1="));
  Serial.print(toggle);
  Serial.print(F(", inputToggle="));
  Serial.println(inputToggle);

  if (inputToggle == 0 && inputLength > 0) {

    inputLength--;
    input[inputLength] = '\0';
    Serial.print(F("inputLength="));
    Serial.print(inputLength);
    Serial.print(F(",input="));
    for (int i = 0; i < inputLength; i++) {
      char c = (char) input[i];
      Serial.write(c);
    }  

    oledPrintInputValue();
  }
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
  int _oldX = cursorX;
  int _oldY = cursorY;
  cursorX = x;
  cursorY = y;
  if (inputToggle == 0) {
    // oledPrint(14, 0, "[  ,  ]", false);
    // oledPrint(15, 0, String(cursorX), false);
    // oledPrint(18, 0, String(cursorY), false);
    oledPrintInputChangeCursor(_oldX, _oldY, cursorX, cursorY);
  }
}
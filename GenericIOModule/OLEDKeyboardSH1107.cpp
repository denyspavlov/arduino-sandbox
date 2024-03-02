
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "OLEDKeyboardSH1107.h"
#include "PushButton.h"
#include "RangeSelector.h"
#include "Log.h"

#define OLED_KEYBOARD "OLED Keyboard v0.01"

OLEDKeyboardSH1107::OLEDKeyboardSH1107(
  int oledClk, int oledMosi, int oledResetPin, int oledDcPin, int oledCsPin, int oledWidth, int oledHeight,
  int ptnPin,  int ptnLow, int ptnHigh,
  int btnPin, int btnInputA1, int btnInputA2, int btnClearA1, int btnClearA2, int btnSelectA1, int btnSelectA2, int btnDeleteA1, int btnDeleteA2, 
  int btnIdleState, unsigned long sensitivity
) : 
  _oled(Adafruit_SH1107(oledWidth, oledHeight, oledMosi, oledClk, oledDcPin, oledResetPin, oledCsPin)),
  _rangeX(oledWidth / 6),
  _rangeY(oledHeight / 10 - 1),
  _rsl(ptnPin, sensitivity, oledWidth / 6, oledHeight / 10 - 1, ptnLow, ptnHigh),
  _mode(btnPin, btnInputA1, btnInputA2, btnIdleState, sensitivity),
  _clr(btnPin, btnClearA1, btnClearA2, btnIdleState, sensitivity),
  _sel(btnPin, btnSelectA1, btnSelectA2, btnIdleState, sensitivity),
  _del(btnPin, btnDeleteA1, btnDeleteA2, btnIdleState, sensitivity) 
{}

void OLEDKeyboardSH1107::_onMode() {
  __logDebug__("OKB:Mode,_modeToggle=", _modeToggle);
  _modeToggle ^= 1;
  if (_modeToggle == 0) {
    _input[0] = '\0';
    _inputLength = 0;
    _onInputModeListener();
    _oled.clearDisplay();
    _oledPrintInput(_cursorX, _cursorY);
  } else {
    _oledPrintWelcome();
    if (_inputLength > 0) {
      _onCaptureListener(&_input[0], _inputLength);
    }
  }

}

void OLEDKeyboardSH1107::_onClr() {
  __logDebug__("OKB:Clr,_modeToggle=", _modeToggle);
  if (_modeToggle == 0) {
    _input[0] = '\0';
    _inputLength = 0;
    _oled.clearDisplay();
    _oled.print(F("X"));
    _oledPrintInput(_cursorX, _cursorY);
  }
}

void OLEDKeyboardSH1107::_onSel() {
  __logDebug__("OKB:Sel,_modeToggle=", _modeToggle);

  if (_modeToggle == 0) {
    byte b = _layout[_cursorY * _rangeX + _cursorX];
    __logDebug__("OKB:Sel,v=", (char) b);

    if (_inputLength == MAX_INPUT) {
      _input[_inputLength - 1] = b;
    } else {
      _input[_inputLength] = b;
      _inputLength++;
      _input[_inputLength] = '\0';
    }

    char _thisInput[MAX_INPUT + 2] = { '\0' };
    int i = 0;
    for (i = 0; i < _inputLength; i++) {
      _thisInput[i] = (char) _input[i];
    }
    _thisInput[i + 1] = '\0';  
    __logDebug__("OKB:Sel,inputLength=", _inputLength, ",input=", _thisInput);

    _oledPrintInputValue();
  }

}

void OLEDKeyboardSH1107::_onDel() {
  __logDebug__("OKB:Del,_modeToggle=", _modeToggle);

  if (_modeToggle == 0 && _inputLength > 0) {

    _inputLength--;
    _input[_inputLength] = '\0';

    char _thisInput[MAX_INPUT + 1] = { '\0' };
    int i = 0;
    for (i = 0; i < _inputLength; i++) {
      _thisInput[i] = (char) _input[i];
    }
    _thisInput[i + 1] = '\0';  
    __logDebug__("OKB:Del,inputLength=", _inputLength, ",input=", _thisInput);

    _oledPrintInputValue();
  }
}

void OLEDKeyboardSH1107::_onRsl(int x, int y) {
  __logDebug__("OKB:Rsl(x,y)=", x, ",", y, ",_modeToggle=", _modeToggle);

  if (_modeToggle == 0) {
    int _oldX = _cursorX;
    int _oldY = _cursorY;
    _cursorX = x;
    _cursorY = y;
    _oledPrintInputChangeCursor(_oldX, _oldY, _cursorX, _cursorY);
  }

}

void OLEDKeyboardSH1107::_oledPrint(int col, int row, String text, bool inverse) {
  _oled.setTextSize(1);
  _oled.setTextColor(inverse ? SH110X_BLACK : SH110X_WHITE, inverse ? SH110X_WHITE : SH110X_BLACK); // black background remove residue text
  _oled.setCursor(col * 6, row * 10);
  _oled.print(text);
  __logDebug__("OKB:Printed(col,row)=", col, ",", row, ",val=", text);
}


void OLEDKeyboardSH1107::_oledPrintWelcome() {
  _oled.clearDisplay();
  _oledPrint(0, 0, OLED_KEYBOARD, false);
  _oledPrint(0, 1, "MOD: input/confirm", false);
  _oledPrint(0, 2, "CLR: clear", false);
  _oledPrint(0, 3, "PTN: navigate", false);
  _oledPrint(0, 4, "SEL: select", false);
  _oledPrint(0, 5, "DEL: delete", false);
  _oled.display();
}


void OLEDKeyboardSH1107::_oledPrintInput(int selectX, int selectY) {
  __logDebug__("OKB:oledPrintInput - start");
  int y,x;
  _oled.setTextSize(1);

  _oled.setCursor(0, 0);
  _oled.setTextColor(SH110X_WHITE, SH110X_BLACK);
  _oled.print('>');

  int startY = 10;

  __logDebug__("OKB:oledPrintInput - select:", selectX, ",", selectY);

  for (y = 0; y < _rangeY; y++) {
    int yRow = startY + y * 10;
    for (x = 0; x < _rangeX; x++) {
      char c = (char) _layout[y * _rangeX + x];
      _oled.setCursor(x * 6, yRow);
      if (y == selectY && x == selectX) {
        _oled.setTextColor(SH110X_BLACK, SH110X_WHITE);
        __logDebug__("OKB:oledPrintInput - y:", y, ",x:", x, ",ch:", c, ",sel:", "*");
      } else {
        _oled.setTextColor(SH110X_WHITE, SH110X_BLACK);
        __logDebug__("OKB:oledPrintInput - y:", y, ",x:", x, ",ch:", c);
      }
      _oled.print(c);
    }
  }
  _oled.display();

  __logDebug__("OKB:oledPrintInput - end");
}


void OLEDKeyboardSH1107::_oledPrintInputChangeCursor(int oldX, int oldY, int selectX, int selectY) {
  __logDebug__("OKB:oledPrintInputChangeCursor - start");

  _oled.setTextSize(1);

  int startY = 10;

  char oldC = (char) _layout[oldY * _rangeX + oldX];
  char newC = (char) _layout[selectY * _rangeX + selectX];

  __logDebug__("OKB:oledPrintInputChangeCursor - select:", selectX, ",", selectY, ",oldChar=", oldC, ",newChar=", newC);

  _oled.setCursor(oldX * 6, startY + oldY * 10);
  _oled.setTextColor(SH110X_WHITE, SH110X_BLACK);
  _oled.print(oldC);
  _oled.setCursor(selectX * 6, startY + selectY * 10);
  _oled.setTextColor(SH110X_BLACK, SH110X_WHITE);
  _oled.print(newC);
  _oled.display();
  
  __logDebug__("OKB:oledPrintInputChangeCursor - end");
}

void OLEDKeyboardSH1107::_oledPrintInputValue() {
  __logDebug__("OKB:oledPrintInputValue - start");

  _oled.setTextSize(1);

  _oled.setCursor(6, 0);
  _oled.setTextColor(SH110X_WHITE, SH110X_BLACK);
  _oled.print("                    ");
  _oled.setCursor(6, 0);
  for (int i = 0; i < _inputLength; i++) {
    char c = (char) _input[i];
    _oled.write(c);
  }  
  _oled.display();
  __logDebug__("OKB:oledPrintInputValue - end");
}

void OLEDKeyboardSH1107::attach(void (*onInputModeListener)(void), void (*onCaptureListener)(char*,int)) {

  __logDebug__("OKB:attach ", OLED_KEYBOARD);

  if (!_oled.begin()) {
    __logError__("OKB:attach - Failed to start SH1107 OLED");
    delay(100);
  }
  delay(100);
  _oled.clearDisplay();
  _oled.display();

  _clr.attach();
  _sel.attach();
  _del.attach();
  _rsl.attach();
  _mode.attach();

  _onInputModeListener = onInputModeListener;
  _onCaptureListener =onCaptureListener;

  __logDebug__("Attaching ... completed");
}

void OLEDKeyboardSH1107::listen() {
  if (_modeToggle == 0) {
    if (_mode.listen()) {
      _onMode();
    }
    if (_clr.listen()) {
      _onClr();
    }
    if (_sel.listen()) {
      _onSel();
      __logDebug__("OK1");
    }
    if (_del.listen()) {
      _onDel();
    }
    RangeSelectorXY xy = _rsl.listen(_rangeX, _rangeY);
    if (xy.updated) {
      _onRsl(xy.x, xy.y);
    }
  }
}

void OLEDKeyboardSH1107::readInput(char prompt[], char layout[], int rangeX, int rangeY) {
  __logDebug__("OKB:readInput,prompt=", prompt, ",layout=", layout, ",rangeX=", rangeX, ",rangeY=", rangeY);
  _layout = layout;
  _prompt = prompt;
  _promptLength = sizeof(prompt) / sizeof(char);
  _rangeX = rangeX;
  _rangeY = rangeY;
  _onMode();
}

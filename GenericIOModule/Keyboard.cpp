
#include <Arduino.h>

#include "Keyboard.h"
#include "Display.h"
#include "PushButton.h"
#include "RangeSelector.h"
#include "Log.h"


Keyboard::Keyboard(
  Display * display,
  int ptnPin,  int ptnLow, int ptnHigh,
  int btnPin, int btnInputA1, int btnInputA2, int btnClearA1, int btnClearA2, int btnSelectA1, int btnSelectA2, int btnDeleteA1, int btnDeleteA2, 
  int btnIdleState, unsigned long sensitivity
) : 
  _display(display),
  _rsl(ptnPin, sensitivity, 1, 1, ptnLow, ptnHigh),
  _mode(btnPin, btnInputA1, btnInputA2, btnIdleState, sensitivity),
  _clr(btnPin, btnClearA1, btnClearA2, btnIdleState, sensitivity),
  _sel(btnPin, btnSelectA1, btnSelectA2, btnIdleState, sensitivity),
  _del(btnPin, btnDeleteA1, btnDeleteA2, btnIdleState, sensitivity) 
{}

void Keyboard::_onMode() {
  __logDebug__("Keyboard:Mode,_modeToggle=", _modeToggle);
  _modeToggle ^= 1;
  if (_modeToggle == 0) {
    _input[0] = '\0';
    _inputLength = 0;
    _onInputModeListener();
    _displayPrintLayout(_cursorX, _cursorY);
  } else {
    _displayClearLayout();
    if (_inputLength > 0) {
      _onCaptureListener(&_input[0], _inputLength);
    }
  }

}

void Keyboard::_onClr() {
  __logDebug__("Keyboard:Clr,_modeToggle=", _modeToggle);
  if (_modeToggle == 0) {
    _input[0] = '\0';
    _inputLength = 0;
    
    //_displayPrintLayout(_cursorX, _cursorY);
    _displayPrintInputValue();
  }
}

void Keyboard::_onSel() {
  __logDebug__("Keyboard:Sel,_modeToggle=", _modeToggle);

  if (_modeToggle == 0) {
    byte b = _layout[_cursorY * _rangeX + _cursorX];
    __logDebug__("Keyboard:Sel,v=", (char) b);

    if (_inputLength == MAX_INPUT) {
      _input[_inputLength - 1] = b;
    } else {
      _input[_inputLength] = b;
      _inputLength++;
      _input[_inputLength] = '\0';
    }

    __logDebug__("Keyboard:Sel,inputLength=", _inputLength, ",input=", _input);

    _displayPrintInputValue();
  }

}

void Keyboard::_onDel() {
  __logDebug__("Keyboard:Del,_modeToggle=", _modeToggle);

  if (_modeToggle == 0 && _inputLength > 0) {

    _inputLength--;
    _input[_inputLength] = '\0';

    char _thisInput[MAX_INPUT + 1] = { '\0' };
    int i = 0;
    for (i = 0; i < _inputLength; i++) {
      _thisInput[i] = (char) _input[i];
    }
    _thisInput[i + 1] = '\0';  
    __logDebug__("Keyboard:Del,inputLength=", _inputLength, ",input=", _thisInput);

    _displayPrintInputValue();
  }
}

void Keyboard::_onRsl(int x, int y) {
  __logDebug__("Keyboard:Rsl(x,y)=", x, ",", y, ",_modeToggle=", _modeToggle);

  if (_modeToggle == 0) {
    int _oldX = _cursorX;
    int _oldY = _cursorY;
    _cursorX = x;
    _cursorY = y;
    _displayPrintLayoutChangeCursor(_oldX, _oldY, _cursorX, _cursorY);
  }

}


void Keyboard::_displayClearLayout() {
  __logDebug__("Keyboard:displayClearLayout - start");

  _display->update(true);

  __logDebug__("Keyboard:displayClearLayout - end");
}


void Keyboard::_displayPrintLayout(int selectX, int selectY) {
  __logDebug__("Keyboard:displayPrintInput - start");
  int y,x;

  // one row above the layout, input marker at position 0
  _display->printChar(0, -_rangeY - 1, '>', false);
  for (x = 1; x < _rangeX; x++) {
    _display->printChar(x, -_rangeY - 1, ' ', false);
  }

  __logDebug__("Keyboard:displayPrintInput - select:", selectX, ",", selectY);

  for (y = 0; y < _rangeY; y++) {
    int yRow = -_rangeY + y;
    for (x = 0; x < _rangeX; x++) {
      char c = (char) _layout[y * _rangeX + x];
      bool highlight = y == selectY && x == selectX;
      __logDebug__("Keyboard:displayPrintInput - y:", y, ",x:", x, ",ch:", c, ",sel:", highlight ? "*" : " ");
      _display->printChar(x, yRow, c, highlight);
    }
  }
  _display->update();

  __logDebug__("Keyboard:displayPrintInput - end");
}


void Keyboard::_displayPrintLayoutChangeCursor(int oldX, int oldY, int selectX, int selectY) {
  __logDebug__("Keyboard:displayPrintInputChangeCursor - start");

  char oldC = (char) _layout[oldY * _rangeX + oldX];
  char newC = (char) _layout[selectY * _rangeX + selectX];

  __logDebug__("Keyboard:displayPrintInputChangeCursor - select:", selectX, ",", selectY, ",oldChar=", oldC, ",newChar=", newC);

  _display->printChar(oldX, -_rangeY + oldY, oldC, false);
  _display->printChar(selectX, -_rangeY + selectY, newC, true);
  _display->update();
  
  __logDebug__("Keyboard:displayPrintInputChangeCursor - end");
}

void Keyboard::_displayPrintInputValue() {
  __logDebug__("Keyboard:displayPrintInputValue - start");

  for (int i = 0; i < _rangeX - 1; i++) {
    char c = i < _inputLength ? (char) _input[i] : ' ';
    _display->printChar(i + 1, -_rangeY - 1, c, false);
  }
  _display->update();

  __logDebug__("Keyboard:displayPrintInputValue - end");
}

void Keyboard::attach(void (*onInputModeListener)(void), void (*onCaptureListener)(char*,int)) {

  __logDebug__("Keyboard:attach - start");

  _clr.attach();
  _sel.attach();
  _del.attach();
  _rsl.attach();
  _mode.attach();

  _onInputModeListener = onInputModeListener;
  _onCaptureListener =onCaptureListener;

  __logDebug__("Keyboard:attach - completed");
}

void Keyboard::listen() {
  if (_modeToggle == 0) {
    if (_mode.listen()) {
      _onMode();
    }
    if (_clr.listen()) {
      _onClr();
    }
    if (_sel.listen()) {
      _onSel();
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

void Keyboard::readInput(char prompt[], char layout[], int rangeX, int rangeY) {
  __logDebug__("Keyboard:readInput,prompt=", prompt, ",layout=", layout, ",rangeX=", rangeX, ",rangeY=", rangeY);
  _display->printText(0, 0, prompt, false);
  _display->update();
  _layout = layout;
  _rangeX = rangeX;
  _rangeY = rangeY;
  _onMode();
}

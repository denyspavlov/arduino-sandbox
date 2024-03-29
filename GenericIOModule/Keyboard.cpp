
#include <Arduino.h>

#include "Keyboard.h"
#include "Display.h"
#include "PushButtonArray.h"
#include "RangeSelector.h"
#include "Log.h"


Keyboard::Keyboard(
  Display * display,
  RangeSelector * rsl,
  PushButtonArray * btns, int btnMode, int btnDel, int btnSel, int btnClr
) : 
  _display(display),
  _rsl(rsl),
  _btns(btns), _btnMode(btnMode), _btnDel(btnDel), _btnSel(btnSel), _btnClr(btnClr)
{}

void Keyboard::_onMode() {
  __logDebug__("Keyboard:Mode,_inputMode=", _inputMode);
  if (_inputMode && _inputLength > 0) {
    _displayClearLayout();
    _onCaptureListener(&_input[0], _inputLength);
    _inputMode = false;
  }

}

void Keyboard::_onClr() {
  __logDebug__("Keyboard:Clr,_inputMode=", _inputMode);
  if (_inputMode) {
    _input[0] = '\0';
    _inputLength = 0;
    _displayPrintInputValue();
  }
}

void Keyboard::_onSel() {
  __logDebug__("Keyboard:Sel,_inputMode=", _inputMode);
  if (_inputMode) {
    char ch = _layout[_cursorY * _rangeX + _cursorX];
    __logDebug__("Keyboard:Sel,v=", ch);

    if (_inputLength == MAX_INPUT) {
      _input[_inputLength - 1] = ch;
    } else {
      _input[_inputLength] = ch;
      _inputLength++;
      _input[_inputLength] = '\0';
    }

    __logDebug__("Keyboard:Sel,inputLength=", _inputLength, ",input=", _input);

    _displayPrintInputValue();
  }

}

void Keyboard::_onDel() {
  __logDebug__("Keyboard:Del,_inputMode=", _inputMode);

  if (_inputMode && _inputLength > 0) {

    _inputLength--;
    _input[_inputLength] = '\0';

    __logDebug__("Keyboard:Del,inputLength=", _inputLength, ",input=", _input);

    _displayPrintInputValue();
  }
}

void Keyboard::_onRsl(int x, int y) {
  __logDebug__("Keyboard:Rsl(x,y)=", x, ",", y, ",_inputMode=", _inputMode);

  if (_inputMode) {
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

  _onInputModeListener = onInputModeListener;
  _onCaptureListener =onCaptureListener;

  __logDebug__("Keyboard:attach - completed");
}

void Keyboard::listen() {
  if (_inputMode) {
    int push = _btns->listen();
    if (push >= 0) {
      __logDebug__("Keyboard:listen,push=", push);
      if (push == _btnMode) {
        _onMode();
      }
      if (push == _btnClr) {
        _onClr();
      }
      if (push == _btnSel) {
        _onSel();
      }
      if (push == _btnDel) {
        _onDel();
      }
    }
    RangeSelectorXY xy = _rsl->listen(_rangeX, _rangeY);
    if (xy.updated) {
      _onRsl(xy.x, xy.y);
    }
  }
}

void Keyboard::readInput(char prompt[], char layout[], int rangeX, int rangeY) {
  __logDebug__("Keyboard:readInput,prompt=", prompt, ",layout=", layout, ",rangeX=", rangeX, ",rangeY=", rangeY);
  
  _display->update(true);
  _display->printText(0, 0, prompt, false);
  
  _layout = layout;
  _rangeX = rangeX;
  _rangeY = rangeY;
  _input[0] = '\0';
  _inputLength = 0;

  _onInputModeListener();
  _displayPrintLayout(_cursorX, _cursorY);

  _inputMode = true;

}

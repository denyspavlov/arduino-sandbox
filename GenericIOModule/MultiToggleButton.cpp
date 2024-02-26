#include <Arduino.h>
#include "MultiToggleButton.h"
#include "Log.h"

MultiToggleButton::MultiToggleButton(int btnPin, int toggles, int idleState, unsigned long sensitivity)
  : _btnPin(btnPin), _toggles(toggles), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState), _toggleIndex(0), _toggleUp(true) {}

void MultiToggleButton::attach(void (*listener)(int)) {
  _listener = listener;
  pinMode(_btnPin, INPUT);
  __logDebug__("MT:btnPin=", _btnPin, ",idleState=", _idleState, ",sensitivity=", _sensitivity);
}

void MultiToggleButton::listen() {
  unsigned long now = millis();
  if (now - _lastReadTime > _sensitivity) {
    int _read = digitalRead(_btnPin);
    int _buttonUp = _idleState ^ 1;
    if (_lastRead == _idleState && _read == _buttonUp) {
      // transition
      if (_toggleUp) {
        // can go up
        if (_toggleIndex < _toggles) {
          _toggleIndex++;
        } else {
          _toggleUp = false;
          _toggleIndex--;
        }
      } else {
        if (_toggleIndex > 0) {
          _toggleIndex--;
        } else {
          _toggleUp = true;
          _toggleIndex++;
        }
      }
      
      __logDebug__("MT:btnPin=", _btnPin, ":->_toggleIndex=", _toggleIndex);
      _listener(_toggleIndex);
    }
    _lastRead = _read;
    _lastReadTime = now;
  }
}

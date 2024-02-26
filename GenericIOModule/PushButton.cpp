#include <Arduino.h>
#include "PushButton.h"
#include "Log.h"

PushButton::PushButton(int btnPin, int idleState, unsigned long sensitivity)
  : _btnPin(btnPin), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState ^ 1), _readA1(-1), _readA2(-1) {}

PushButton::PushButton(int btnPin, int readA1, int readA2, int idleState, unsigned long sensitivity)
  : _btnPin(btnPin), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState ^ 1), _readA1(readA1), _readA2(readA2) {}

void PushButton::attach(void (*listener)(void)) {
  _listener = listener;
  pinMode(_btnPin, INPUT);
  __logDebug__("PB:btnPin=", _btnPin, ",idleState=", _idleState, ",sensitivity=", _sensitivity);
  __logDebug__("PB:btnPin=", _btnPin, ",readA1=", _readA1, ",readA2=", _readA2);
}

bool PushButton::listen() {
  unsigned long now = millis();
  bool pushed = false;
  if (now - _lastReadTime > _sensitivity) {
    if (_readA1 >= 0 && _readA2 >= 0) { // analog (multi button pin)
      int _readA = analogRead(_btnPin);
      int _read = _readA >= _readA1 && _readA <= _readA2 ? _idleState ^ 1 : _idleState;
      int _buttonUp = _idleState ^ 1;
      if (_lastRead == _idleState && _read == _buttonUp) {
        // transition
        pushed = true;      
        __logDebug__("PB(A):btnPin=", _btnPin, ":->pressed=", 1, ",_readA=", _readA);
      }
      _lastRead = _read;
    } else { // digital (one button pin)
      int _read = digitalRead(_btnPin);
      int _buttonUp = _idleState ^ 1;
      if (_lastRead == _idleState && _read == _buttonUp) {
        // transition
        pushed = true;      
        __logDebug__("PB(D):btnPin=", _btnPin, ":->pressed=", 1, ",_read=", _read);
      }
      _lastRead = _read;
    }
    _lastReadTime = now;
  }
  if (pushed && _listener != nullptr) {
    _listener();
  }
  return pushed;
}

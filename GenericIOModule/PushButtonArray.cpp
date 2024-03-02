#include <Arduino.h>
#include "PushButtonArray.h"
#include "Log.h"

PushButtonArray::PushButtonArray(int btnPin, int btnCount, int * readA, int idleState, unsigned long sensitivity)
  : _btnPin(btnPin), _btnCount(btnCount), _readA(readA), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState ^ 1), _lastReadIndex(-1) {}


void PushButtonArray::attach(void (*listener)(int)) {
  _listener = listener;
  pinMode(_btnPin, INPUT);
  __logDebug__("PBA:btnPin=", _btnPin, ",idleState=", _idleState, ",sensitivity=", _sensitivity, ",btnCount=", _btnCount);
}

int PushButtonArray::listen() {
  unsigned long now = millis();
  bool pushed = false;
  int _i = -1;
  if (now - _lastReadTime > _sensitivity) {
    int _a = analogRead(_btnPin);
    for (int i = 0; i < _btnCount; i++) {
      if (_a >= _readA[2 * i] && _a <= _readA[2 * i + 1]) {
        _i = i;
        break;
      }
    }

    int _read = _i >= 0 ? _idleState ^ 1 : _idleState;
    int _buttonUp = _idleState ^ 1;
    if (_lastReadIndex == _i && _read == _buttonUp) {
      // transition
      pushed = true;      
      __logDebug__("PBA:btnPin=", _btnPin, ":->pressed=", _i, ",_readA=", _a);
    }
    _lastRead = _read;
    _lastReadIndex = _i;
    _lastReadTime = now;
  }
  if (pushed) {
    if (_listener != nullptr) {
      _listener(_i);
    }
    return _i;
  }
  return -1;
}

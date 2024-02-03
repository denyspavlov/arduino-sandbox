#include <Arduino.h>
#include "MultiToggleButton.h"

MultiToggleButton::MultiToggleButton(int btnPin, int toggles, int idleState = LOW, unsigned long sensitivity = 100)
  : _btnPin(btnPin), _toggles(toggles), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState), _toggleIndex(0), _toggleUp(true) {}

void MultiToggleButton::attach(void (*listener)(int)) {
  _listener = listener;
  pinMode(_btnPin, INPUT);
  Serial.print(F("btnPin="));
  Serial.print(_btnPin);
  Serial.print(F(",idleState="));
  Serial.print(_idleState);
  Serial.print(F(",sensitivity="));
  Serial.println(_sensitivity);
}

int MultiToggleButton::getToggle() {
  unsigned long now = millis();
  int diffInSeconds = now - _lastRead;
  if (now - _lastRead > _sensitivity) {
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
      
      Serial.print(F("btnPin="));
      Serial.print(_btnPin);
      Serial.print(F("_toggleIndex="));
      Serial.println(_toggleIndex);
      _listener(_toggleIndex);
    }
    _lastRead = _read;
    _lastReadTime = now;
  }

  return _toggleIndex;
}

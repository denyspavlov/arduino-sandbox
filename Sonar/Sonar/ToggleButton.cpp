#include <Arduino.h>
#include "ToggleButton.h"

ToggleButton::ToggleButton(int btnPin, int idleState = LOW, unsigned long sensitivity = 100)
  : _btnPin(btnPin), _idleState(idleState), _sensitivity(sensitivity), _lastRead(idleState), _toggleOn(false) {}

void ToggleButton::attach(void (*listener)(bool)) {
  _listener = listener;
  pinMode(_btnPin, INPUT);
  Serial.print(F("btnPin="));
  Serial.print(_btnPin);
  Serial.print(F(",idleState="));
  Serial.print(_idleState);
  Serial.print(F(",sensitivity="));
  Serial.println(_sensitivity);
}

bool ToggleButton::isToggleOn() {
  unsigned long now = millis();
  int diffInSeconds = now - _lastRead;
  if (now - _lastRead > _sensitivity) {
    int _read = digitalRead(_btnPin);
    int _buttonUp = _idleState ^ 1;
    if (_lastRead == _idleState && _read == _buttonUp) {
      // transition to ON
      _toggleOn ^= 1;
      Serial.print(F("btnPin="));
      Serial.print(_btnPin);
      Serial.print(F("toggleOn="));
      Serial.println(_toggleOn);
      _listener(_toggleOn);
    }
    _lastRead = _read;
    _lastReadTime = now;
  }

  return _toggleOn;
}

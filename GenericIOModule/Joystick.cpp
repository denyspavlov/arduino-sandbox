#include <Arduino.h>
#include "Joystick.h"
#include "Log.h"

Joystick::Joystick(int btnPin, int xPin, int yPin, unsigned long sensitivity, int rangeX, int rangeY)
  : _btn(new MultiToggleButton(btnPin, 1, HIGH, sensitivity)), _xPin(xPin), _yPin(yPin), _sensitivity(sensitivity), _rangeX(rangeX), _rangeY(rangeY) {}

void Joystick::attach(void (*btnListener)(int), void (*axisListener)(int, int)) {
  _axisListener = axisListener;
  _btn->attach(btnListener);
  __logDebug__("J:xPin=", _xPin, ",yPin=", _yPin, ",rangeX=", _rangeX, ",rangeY=", _rangeY);
}

void Joystick::listen() {
  unsigned long now = millis();
  if (now - _lastReadTime > _sensitivity) {
    int _readX = map(analogRead(_xPin), 0, 1023, 0, _rangeX);
    int _readY = map(analogRead(_yPin), 0, 1023, 0, _rangeY);
    if (_lastReadX != _readX || _lastReadY != _readY) {
      // changed position
      _x = _readX;
      _y = _readY;

      __logDebug__("J:jPin=", _xPin, ",", _yPin, ":->(x,y))=", _x, ",", _y);
      _axisListener(_x, _y);
    }

    _lastReadX = _readX;
    _lastReadY = _readY;
    _lastReadTime = now;
  }
}

#include <Arduino.h>
#include "RangeSelector.h"

RangeSelector::RangeSelector(int ptnPin, unsigned long sensitivity, int rangeX, int rangeY)
  : _ptnPin(ptnPin), _sensitivity(sensitivity), _rangeX(rangeX), _rangeY(rangeY) {}

void RangeSelector::attach(void (*axisListener)(int, int)) {
  _axisListener = axisListener;
  Serial.print(F("ptnPin="));
  Serial.print(_ptnPin);
  Serial.print(F(",rangeX="));
  Serial.print(_rangeX);
  Serial.print(F(",rangeY="));
  Serial.println(_rangeY);
}

void RangeSelector::listen() {
  unsigned long now = millis();
  if (now - _lastReadTime > _sensitivity) {
    int _read = map(analogRead(_ptnPin), 0, 1023, 0, _rangeX * _rangeY - 1);
    if (_lastRead != _read) {
      // changed position
      _x = _read % _rangeX;
      _y = _read / _rangeX;

      Serial.print(F("ptnPin="));
      Serial.print(_ptnPin);
      Serial.print(F(",_read="));
      Serial.print(_read);
      Serial.print(F(" :->(x,y))="));
      Serial.print(_x);
      Serial.print(F(","));
      Serial.println(_y);

      _axisListener(_x, _y);
    }

    _lastRead = _read;
    _lastReadTime = now;
  }
}

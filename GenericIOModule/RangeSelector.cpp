#include <Arduino.h>
#include "RangeSelector.h"
#include "Log.h"

RangeSelector::RangeSelector(int ptnPin, unsigned long sensitivity, int rangeX, int rangeY, int readA1, int readA2)
  : _ptnPin(ptnPin), _sensitivity(sensitivity), _rangeX(rangeX), _rangeY(rangeY), _readA1(readA1), _readA2(readA2) {}

void RangeSelector::attach(void (*axisListener)(int, int)) {
  _axisListener = axisListener;
  __logDebug__("RS:ptnPin=", _ptnPin, ",rangeX=", _rangeX, ",rangeY=", _rangeY);
  __logDebug__("PB:ptnPin=", _ptnPin, ",readA1=", _readA1, ",readA2=", _readA2);
}

RangeSelectorXY RangeSelector::listen(int rangeX, int rangeY) {
  RangeSelectorXY xy = { _x, _y, false };
  bool updated = false;
  unsigned long now = millis();
  if (now - _lastReadTime > _sensitivity) {
    int _useRangeX = rangeX > 0 ? rangeX : _rangeX;
    int _useRangeY = rangeY > 0 ? rangeY : _rangeY;
    int _raw = analogRead(_ptnPin);
    int _read = map(_raw, _readA1, _readA2, 0, _useRangeX * _useRangeY - 1);
    if (_lastRead != _read) {
      // changed position
      _x = _read % _useRangeX;
      _y = _read / _useRangeX;
      __logDebug__("RS:ptnPin=", _ptnPin, ",_useRangeX=", _useRangeX, ",_useRangeY=", _useRangeY);
      __logDebug__("RS:ptnPin=", _ptnPin, ",_raw=", _raw, ",_read=", _read);
      __logDebug__("RS:ptnPin=", _ptnPin, ",_read=", _read, ":->(x,y))=", _x, ",", _y);
      updated = true;
      xy.x = _x;
      xy.y = _y;
      xy.updated = true;
    }

    _lastRead = _read;
    _lastReadTime = now;
  }
  if (updated && _axisListener != nullptr) {
      _axisListener(_x, _y);
  }
  return xy;
}

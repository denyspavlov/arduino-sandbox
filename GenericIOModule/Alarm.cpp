#include <Arduino.h>
#include "Alarm.h"
#include "Log.h"

Alarm::Alarm(int buzzPin, unsigned long interval)
  : _buzzPin(buzzPin), _interval(interval) {}


void Alarm::attach(void (*listener)(bool)) {
  _listener = listener;
  pinMode(_buzzPin, OUTPUT);
  __logDebug__("AL:_buzzPin=", _buzzPin, ",_on=", _on, ",_interval=", _interval);
}

bool Alarm::listen() {
  unsigned long now = millis();
  bool change = false;
  if (now - _lastWriteTime > _interval) {
    if (_switchToAfter > 0) { // need to change state
      if (_switchTo != _on && now > _switchToAfter) {
        _on = _switchTo;
        _switchToAfter = -1;
        change = true;
        __logDebug__("AL:listen,switched,_buzzPin=", _buzzPin, ",_on=", _on);
      }
    }
    if (_on) {
      // Alternate between high and low to produce beeping (interval)
      int _w = _lastWrite == LOW ? HIGH : LOW;
      digitalWrite(_buzzPin, _w);
      _lastWrite = _w;
      _lastWriteTime = now;
      __logDebug__("AL:listen,_buzzPin=", _buzzPin, ",_on=", _on, ",_pinWrite=", _w);
    } else if (_lastWrite != LOW) {
      // make sure it is OFF
      digitalWrite(_buzzPin, LOW);
      _lastWrite = LOW;
      _lastWriteTime = now;
      __logDebug__("AL:listen,_buzzPin=", _buzzPin, ",_on=", _on, ",_pinWrite=", LOW);
    }
    if (change && _listener != nullptr) {
      _listener(_on);
    }
  }
  return change;
}

bool Alarm::toggle(bool on, unsigned long duration) {
  bool change = _on != on;
  if (!on) { // force silence buzzer
    digitalWrite(_buzzPin, LOW);
    _lastWrite = LOW;
    _lastWriteTime = millis();
    __logDebug__("AL:listen,_buzzPin=", _buzzPin, ",_on=", _on, ",_pinWrite=", LOW);
  }
  if (change) {
    _on = on;
    _switchTo = !on;
    if (duration > 0) {
      _switchToAfter = millis() + duration;
    } else {
      _switchToAfter = -1;
    }
    __logDebug__("AL:toggle=", on, ",_switchTo=", _switchTo, ",_switchToAfter=", _switchToAfter, ",duration=", duration);
    if (_listener != nullptr) {
      _listener(_on);
    }
  }
  return change;
}

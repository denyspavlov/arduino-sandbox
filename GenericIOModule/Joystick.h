#ifndef __JOYSTICK__
#define __JOYSTICK__

#include <Arduino.h>
#include "MultiToggleButton.h"

/**
 * Joystick with 2x analog reads and one select button
 */
class Joystick {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin     - Button pin (backed by a PullUp resistor)
   * xPin       - X analog pin
   * yPin       - Y analog pin
   * sensitivity - timeout between reads (bounce in a switch)
   */
  Joystick(int btnPin, int xPin, int yPin, unsigned long sensitivity = 100, int rangeX = 20, int rangeY = 4);
  /**
   * Hardware initialization. Use in setup().
   * 
   * btnListener   - callback function that will receive pressed.
   * axisListener  - callback function that will trigger if axis change
   */
  void attach(void (*btnListener)(int), void (*axisListener)(int, int));

  /**
   * Takes reading and saves toggle value
   */
  void listen();

private:
  const MultiToggleButton *_btn;
  const int _xPin;
  const int _yPin;
  const unsigned long _sensitivity;
  const int _rangeX;
  const int _rangeY;
  int _lastReadX = 512;
  int _lastReadY = 512;
  unsigned long _lastReadTime = 0;
  int _x = 512;
  int _y = 512;
  void (*_axisListener)(int, int);
};

#endif
#ifndef __PUSH_BUTTON_ARRAY__
#define __PUSH_BUTTON_ARRAY__

#include <Arduino.h>

/**
 * Push button
 */
class PushButtonArray {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin      - Button pin (backed by a PullUp resistor)
   * btnCount    - how many buttons in the array
   * readA       - analog read ranges, specified in pairs for each button
   * idleState   - LOW or HIGH (when not pressed)
   * sensitivity - timeout between reads (bounce in a switch)
   */
  PushButtonArray(int btnPin, int btnCount, int * readA, int idleState = HIGH, unsigned long sensitivity = 100);

  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive button index as input
   */
  void attach(void (*listener)(int) = nullptr);

  /**
   * Takes reading and determines if "push" event occured
   * If listener is provided, triggers the listener if pushed=true
   * int=-1, no input, int>=0 button push for button index detected
   */
  int listen();

private:
  const int _btnPin;
  const int _btnCount;
  const int * _readA;
  const int _idleState;
  const unsigned long _sensitivity;
  int _lastRead = HIGH;
  int _lastReadIndex = -1;
  unsigned long _lastReadTime = 0;
  void (*_listener)(int);
};

#endif
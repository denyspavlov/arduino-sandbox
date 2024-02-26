#ifndef __PUSH_BUTTON__
#define __PUSH_BUTTON__

#include <Arduino.h>

/**
 * Push button
 */
class PushButton {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin      - Button pin (backed by a PullUp resistor)
   * idleState   - LOW or HIGH (when not pressed)
   * sensitivity - timeout between reads (bounce in a switch)
   */
  PushButton(int btnPin, int idleState = HIGH, unsigned long sensitivity = 100);

  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin      - Button pin (backed by a PullUp resistor)
   * readA1      - analog mode read low
   * readA2      - analog mode read high
   * idleState   - LOW or HIGH (when not pressed)
   * sensitivity - timeout between reads (bounce in a switch)
   */
  PushButton(int btnPin, int readA1, int readA2, int idleState = HIGH, unsigned long sensitivity = 100);

  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive toggleFlag as an input
   */
  void attach(void (*listener)(void) = nullptr);

  /**
   * Takes reading and determines if "push" event occured
   * If listener is provided, triggers the listener if pushed=true
   */
  bool listen();

private:
  const int _btnPin;
  const int _idleState;
  const unsigned long _sensitivity;
  int _lastRead = HIGH;
  const int _readA1 = -1;
  const int _readA2 = -1;
  unsigned long _lastReadTime = 0;
  void (*_listener)(void);
};

#endif
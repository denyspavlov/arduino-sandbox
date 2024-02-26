#ifndef __MULTI_TOGGLE_BUTTON__
#define __MULTI_TOGGLE_BUTTON__

#include <Arduino.h>

/**
 * Toggle button with toggles configurable parameter
 */
class MultiToggleButton {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin     - Button pin (backed by a PullUp resistor)
   * toggles    - levels to toggle (for ON/OFF toggles=1)
   * idleState  - LOW or HIGH (when not pressed)
   * sensitivity - timeout between reads (bounce in a switch)
   */
  MultiToggleButton(int btnPin, int toggles, int idleState = HIGH, unsigned long sensitivity = 100);
  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive toggleFlag as an input
   */
  void attach(void (*listener)(int));

  /**
   * Takes reading and saves toggle value
   */
  void listen();

private:
  const int _btnPin;
  const int _idleState;
  const unsigned long _sensitivity;
  int _lastRead = HIGH;
  unsigned long _lastReadTime = 0;
  int _toggles = 1;
  int _toggleIndex = 0;
  bool _toggleUp = true;
  void (*_listener)(int);
};

#endif
#ifndef __MULTI_TOGGLE_BUTTON__
#define __MULTI_TOGGLE_BUTTON__

#include <Arduino.h>

/**
 * Bespoke version of
 * //
 * pinMode(ON_BTN, INPUT);
 * attachInterrupt(digitalPinToInterrupt(ON_BTN), onBtnTogglePress, CHANGE);
 * //
 * Note that only specific pins work with interrupts, consult the board sheet
 * There are very few on the "smaller" boards
 */
class MultiToggleButton {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * btnPin     - Button pin (backed by a PullUp resistor)
   * toggles    - levels to toggle
   * idleState  - LOW or HIGH (when not pressed)
   * sensitivity - timeout between reads (bounce in a switch)
   */
  MultiToggleButton(int btnPin, int toggles, int idleState = LOW, unsigned long sensitivity = 100);
  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive toggleFlag as an input
   */
  void attach(void (*listener)(int));

  /**
   * Check if the current state of the toggle is ON.
   */ 
  int getToggle();

private:
  const int _btnPin;
  const int _idleState;
  const unsigned long _sensitivity;
  int _lastRead = LOW;
  unsigned long _lastReadTime = 0;
  int _toggles = 1;
  int _toggleIndex = 0;
  bool _toggleUp = true;
  void (*_listener)(int);
};

#endif
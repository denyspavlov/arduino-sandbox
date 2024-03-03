#ifndef __SITE_CHECKER__
#define __SITE_CHECKER__

#include <Arduino.h>

/**
 * Alarm
 */
class Alarm {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * buzzPin      - Buzzer pin
   * period       - timeout between sound on/off
   */
  Alarm(int buzzPin, unsigned long interval = 50);

  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive toggleFlag as an input
   */
  void attach(void (*listener)(bool) = nullptr);

  /**
   * Takes reading and determines if "push" event occured
   * If listener is provided, triggers the listener if buzzing toggle changes
   */
  bool listen();

  /**
   * Switch the buzzer on/of
   *
   * on       - true to turn on, false to turn off
   * duration - ms to switch automatically to the opposite after duration
   */
  bool toggle(bool on, unsigned long duration = -1);

private:
  const int _buzzPin;
  const unsigned long _interval;
  bool _on = false;
  int _lastWrite = LOW;
  unsigned long _lastWriteTime = 0;
  bool _switchTo = false;
  unsigned long _switchToAfter = 0;
  void (*_listener)(bool);
};


#endif
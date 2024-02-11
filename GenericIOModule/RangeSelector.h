#ifndef __RANGE_SELECTOR__
#define __RANGE_SELECTOR__

#include <Arduino.h>

/**
 * Potentiometer selector that can remap to a matrix
 */
class RangeSelector {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * ptnPin      - Potentiometer pin
   * sensitivity - timeout between reads
   * rangeX      - range of X value
   * rangeY      - range of Y value
   */
  RangeSelector(int ptnPin, unsigned long sensitivity = 100, int rangeX = 20, int rangeY = 4);
  /**
   * Hardware initialization. Use in setup().
   * 
   * axisListener  - callback function that will trigger if axis change
   */
  void attach(void (*axisListener)(int, int));

  /**
   * Takes reading and saves toggle value
   */
  void listen();

private:
  const int _ptnPin;
  const unsigned long _sensitivity;
  const int _rangeX;
  const int _rangeY;
  int _lastRead = 0;
  unsigned long _lastReadTime = 0;
  int _x = 0;
  int _y = 0;
  void (*_axisListener)(int, int);
};

#endif
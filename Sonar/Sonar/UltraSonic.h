#ifndef __ULTRA_SONIC__
#define __ULTRA_SONIC__

class UltraSonic {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * trigPin - Trig pin
   * echoPin - Echo pin
   */
  UltraSonic(int trigPin, int echoPin);
  /**
   * Hardware initialization. Use in setup().
   */
  void attach();
  /**
   * Perform a reading: 
   * 1. trigger LOW 2 mS
   * 2. trigger HIGH 10 mS
   * 3. trigger LOW + pulseIn
   * Total time ~20mS
   */
  float getDistanceCm();

private:
  unsigned long getDurationUs();
  const int _trigPin;
  const int _echoPin;
};

#endif
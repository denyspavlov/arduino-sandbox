#include <Arduino.h>
#include "UltraSonic.h"

// Speed of sound 343 m / s in dry air at 20 °C
// 343*100 = 34,300 cm / s
// 34,300 cm / s / 1,000,000 = 0.0343 cm / mS
// 0.0343 / 2 to compensation for return journey = 0.01715
constexpr float SOUND_SPEED_CM_PER_MS = 0.01715f;

UltraSonic::UltraSonic(int trigPin, int echoPin)
  : _trigPin(trigPin), _echoPin(echoPin) {}

void UltraSonic::attach() {
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  Serial.print(F("trigger="));
  Serial.print(_trigPin);
  Serial.print(F(",echo="));
  Serial.println(_echoPin);
}

/*
  Overview:

  1. a short low pulse (less than 10μs) is sent out on the trigger pin (connected to _trigPin) to ensure 
     that the sensor is idle.

  2. a high pulse lasting 10μs is sent out on the trigger pin to start the ranging cycle. Upon receiving 
     this rising edge, the sensor starts generating eight cycles of ultrasound waves with a frequency 
     around 40kHz.

  3. Once the ultrasound burst leaves the emitter, it travels through air until reflected back by the 
     nearest obstacle. Some portion of the wave gets absorbed and scattered along the way due to environmental 
     factors.

  4. The reflected sound reaches the receiver unit, inducing a voltage variation on the echo pin (connected to _echoPin). 
     At this point, another pulse begins on the echo pin.

  5. Measurement of the width of this returning pulse provides information about the distance between the sensor and the 
     target. Specifically, the duration corresponds directly to twice the travel time of the sound wave — one round trip, 
     accounting for propagation times to reach the target and come back to the sensor. 
     340 m/s / 2 converted to cm/mS
 */
unsigned long UltraSonic::getDurationUs() {
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2); // sensor is idle

  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10); // high pulse
  digitalWrite(_trigPin, LOW);

  unsigned long us = pulseIn(_echoPin, HIGH, 50000); // make sure we set timeout to 50 millis
  return us;
}

float UltraSonic::getDistanceCm() {
  float cm = getDurationUs() * SOUND_SPEED_CM_PER_MS;
  if (cm < 2.0f) {
    Serial.print(F("Too close (<2): cm="));
    Serial.println(cm);
  } else if (cm > 400.0f) {
    Serial.print(F("Too far (>400): cm="));
    Serial.println(cm);
  } else {
    Serial.print(F("Measured: cm="));
    Serial.println(cm);
  }
  return cm;
}

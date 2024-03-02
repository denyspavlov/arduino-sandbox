#include <Arduino.h>

#ifdef __AVR__
  #define LED_BUILTIN 13
#elif defined(ESP32)
  #define LED_BUILTIN 2
#elif defined(ESP8266)
  #define LED_BUILTIN 2
#endif

/**
 * Blank sketch to reset MCU
 */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Device is operational"));
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(15, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(F("Device is operational"));
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(15, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(15, LOW);
  delay(300);
}

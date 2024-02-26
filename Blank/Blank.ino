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

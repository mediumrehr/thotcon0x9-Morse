#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  EEPROM.begin(512);
  delay(100);
  
  pinMode(4, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(12, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  t();
  h();
  o();
  t();
  c();
  o();
  n();
  delay(1000);
}

void morseShort() {
  analogWrite(12,51);
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  analogWrite(12,0);
  delay(50);
}

void morseLong() {
  analogWrite(12,51);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  analogWrite(12,0);
  delay(50);
}

void t() {
  morseLong();
  delay(50);
}

void h() {
  morseShort();
  morseShort();
  morseShort();
  morseShort();
  delay(50);
}

void o() {
  morseLong();
  morseLong();
  morseLong();
  delay(50);
}

void c() {
  morseLong();
  morseShort();
  morseLong();
  morseShort();
  delay(50);
}

void n() {
  morseLong();
  morseShort();
  delay(50);
}


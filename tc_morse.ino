/*
 * thotcon badge morse code repeater
 * rob rehr (@mediumrehr)
 */

// first byte is number of notes
// second two bytes are binary representation of morse code symbol
//   1: long
//   2: short
uint16_t morse[36] = {0x201,0x408,0x40a,0x304,0x100,0x402,0x306,0x400,0x200,0x407,  // A - J
                      0x305,0x404,0x203,0x202,0x307,0x406,0x40d,0x302,0x300,0x101,  // K - T
                      0x301,0x401,0x303,0x409,0x40b,0x40c,                          // U - Z
                      0x50f,0x507,0x503,0x501,0x500,0x510,0x518,0x51c,0x51e,0x51f}; // 0 - 9

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  analogWrite(12, 0);
  digitalWrite(13, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  string2Morse("thotcon");
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

void char2Morse(char c) {
  uint8_t charVal = 0;
  if (c >= 65) {
    // filter out case
    charVal = c - 65;
    if (charVal > 25) {
      charVal -= 32;
      if ((charVal > 25) || (charVal < 0))
        return; // not a letter
    }
  } else {
    // check if number
    charVal = c - 48;
    if (charVal < 10) {
      charVal += 26;
    } else
      return; // not a number
  }
  
  // parse morse code playback data for charVal
  uint16_t value = morse[charVal];
  uint8_t morseLen = value >> 8;
  uint8_t morseVal = value & 0x0FF;

  for (int i=1; i<=morseLen; i++) {
    uint8_t morseNote = (morseVal >> (morseLen - i)) & 0x1;
    if (morseNote)
      morseLong();
    else
      morseShort();
  }
  delay(300); // short delay between letters
}

void string2Morse(char* str) {
  uint16_t strLen = strlen(str);
  for (int i=0; i<strLen; i++) {
    char c = str[i];
    //Serial.println(c);
    if (c == ' ')
      delay(500); // pause between words
    else
      char2Morse(str[i]);
  }
}

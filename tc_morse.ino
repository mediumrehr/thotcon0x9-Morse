/*
 * thotcon badge morse code repeater
 * rob rehr (@mediumrehr)
 */

#include <U8g2lib.h>

#define DOWNPIN 0
#define SELPIN  4
#define UPPIN   5

// first byte is number of notes
// second two bytes are binary representation of morse code symbol
//   1: long
//   2: short
uint16_t morse[36] = {0x201,0x408,0x40a,0x304,0x100,0x402,0x306,0x400,0x200,0x407,  // A - J
                      0x305,0x404,0x203,0x202,0x307,0x406,0x40d,0x302,0x300,0x101,  // K - T
                      0x301,0x401,0x303,0x409,0x40b,0x40c,                          // U - Z
                      0x50f,0x507,0x503,0x501,0x500,0x510,0x518,0x51c,0x51e,0x51f}; // 0 - 9

// 0: basic select
// 1: edit string
uint8_t menuMode = 0;

char *menu[] = {"play", "change", "load", "save"};
uint8_t menuSize = sizeof(menu) / sizeof(menu[0]);
int menuSel = 0;

char** pMenu;
void (*pHandler)(int);

#define MAXSTRLEN 15 // max of 15 to fit on screen
char morseStr[MAXSTRLEN+1] = "thotcon";
int strSel = -1; // start with character highlight hidden

// display instance
U8G2_UC1701_EA_DOGS102_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 12);

void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
  
  Serial.begin(9600);
  
  pinMode(UPPIN, INPUT_PULLUP);
  pinMode(SELPIN, INPUT_PULLUP);
  pinMode(DOWNPIN, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  analogWrite(12, 0);
  digitalWrite(13, HIGH);

  pMenu = menu;
  pHandler = &mainHandler;
  updateDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(UPPIN)) {
    menuSel++;
    if (menuSel > (menuSize - 1)) {
      menuSel = 0;
    }
    updateDisplay();
    delay(150); // debounce
  }
  
  else if (!digitalRead(DOWNPIN)) {
    menuSel--;
    if (menuSel < 0) {
      menuSel = menuSize - 1;
    }
    updateDisplay();
    delay(150); // debounce
  }
  
  else if (!digitalRead(SELPIN)) {
    pHandler(menuSel);
    delay(150); // debounce
  }
}

/*** Morse code stuff ***/

/* play a short pulse */
void morseShort() {
  analogWrite(12,51);
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  analogWrite(12,0);
  delay(50);
}

/* play a long pulse */
void morseLong() {
  analogWrite(12,51);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  analogWrite(12,0);
  delay(50);
}

/* convert a character to morse code pulses */
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

/* convert a string to morse code pulses */
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

/*** Display stuff ***/

/* display morse code string with highlighted letter */
void displayString(char* str, int selection) {
  // set draw settings
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setFontPosTop();
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);
  
  // draw character selection bar
  // selection set to -1 means hide selection bar
  if (selection != -1) {
    u8g2.drawBox(5 + (selection*6), 6, 7, 10);
  }

  // draw string
  u8g2.drawStr(6,6,str);
}

/* center string in x and display it at y */
void drawStrCentered(int y, char* str) {
  uint8_t strLen = strlen(str);
  uint8_t x = (102 - (strLen*6))/2;
  u8g2.drawStr(x,y,str);
}

/* display a selectable menu */
void displayMenu(char* options[], uint8_t numOptions, void (*handler)(int), int selection, uint8_t yOffset) {
  // set draw settings
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setFontPosTop();
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);

  // draw selection bar
  // selection set to -1 means hide selection bar
  if (selection != -1) {
    u8g2.drawBox(2, yOffset+(selection*10), 98, 10);
  }

  // draw selectable options
  for (int i=0; i<numOptions; i++) {
    drawStrCentered(yOffset + (i*10), options[i]);
  }
}

/* update morse string and menu displays */
void updateDisplay() {
  u8g2.clearBuffer(); // clear the internal memory
  displayString(morseStr,strSel);
  displayMenu(pMenu,menuSize,pHandler,menuSel,20);
  u8g2.sendBuffer(); // send buffer to screen
}

/*** Handler functions ***/

/* handle main menu selections */
void mainHandler(int selection) {
  switch(menuSel) {
    case 0: // play string
      string2Morse(morseStr);
      break;
    case 1: // change string
      menuSel = -1; // hide menu selector
      strSel = strlen(morseStr);
      if (strSel > (MAXSTRLEN - 1)) {
        strSel = MAXSTRLEN - 1;
      }
      updateDisplay();
      delay(150); // debounce
      strEditor();
      delay(150); // debounce
      break;
    case 2: // load string from eeprom
      break;
    case 3: // save string to eeprom
      break;
    default:
      break;
  }
}

/* handle editing morse code string */
void strEditor() {
  char currLetter = 0;
  while (1) {
    if (!digitalRead(UPPIN)) {
      if (currLetter != 0) {
        strSel++;
        if (strSel > (MAXSTRLEN - 1)) {
          strSel = -1;
          menuSel = 0;
          updateDisplay();
          return;
        }
      } else {
        strSel = -1;
        menuSel = 0;
        updateDisplay();
        return;
      }
      currLetter = 0;
      updateDisplay();
      delay(150); // debounce
    } 
    
    else if (!digitalRead(DOWNPIN)) {
      morseStr[strSel] = 0;
      strSel--;
      currLetter = morseStr[strSel];
      if (strSel < 0) {
        strSel = strlen(morseStr);
      }
      updateDisplay();
      delay(150); // debounce
    }
    
    else if (!digitalRead(SELPIN)) {
      currLetter++;
      if (currLetter == 1) {
        currLetter = 'a';
      } else if (currLetter > 'z') {
        currLetter = '0';
      } else if ((currLetter > '9') && (currLetter < 'a')) {
        currLetter = 0; // back to ' as empty char
      }
      morseStr[strSel] = currLetter;
      updateDisplay();
      delay(150); // debounce
    }
  }
}


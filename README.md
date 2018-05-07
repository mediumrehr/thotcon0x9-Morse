# Thotcon 0x9 badge hack

Programmed in [Arduino IDE](https://www.arduino.cc/en/Main/Software) 1.8.3

Board: [SparkFun ESP8266 Thing Dev](https://learn.sparkfun.com/tutorials/esp8266-thing-development-board-hookup-guide/setting-up-arduino)

## Connections

GPIO 13 (7th via from the top on right): Connect to positive side of LED (negative LED side to GND)

GPIO 12 (8th via from the top on right): Connect to input Adafruit MAX98306 Amp (differential side to GND)

## other

Ignore/delete the eeprom stuff for now. Might add support to store/recall morse strings in the future.
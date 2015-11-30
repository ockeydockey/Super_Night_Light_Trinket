/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 ******************************************/
#include <EEPROM.h>

#define CdS 2
#define BUTTON 3
#define LED 1

// LED Modes
#define OFF 0
#define FADINGON 1
#define ON 2
#define FADINGOFF 3

// Operation Modes
#define SHORT 0
#define LONG 1
// Operation Mode Times (in miliseconds)
#define SHORT_TIME 15000
#define LONG_TIME 60000

unsigned int threshold = 150;

bool lightSensed;
bool triggered;

byte LEDMode;
byte LEDBrightness;
byte operationMode;
unsigned long startTime;
unsigned long endTime;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(CdS, INPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);

  LEDMode = OFF;
  LEDBrightness = 0;
  startTime = 0;
  endTime = 0;

  triggered = false;

  // Retreive previous setting, and set to default if no value was set
  EEPROM.get(0x0, operationMode);
  if (operationMode == 255) {
    operationMode = 0;
  }
}

void manualControl() {
  startTime = millis();
  for (byte i = 0; i < 255; i++) {
    analogWrite(LED, i);
    delay(2);
  }
  digitalWrite(LED, HIGH);
  endTime = startTime + 3000;
  while (digitalRead(BUTTON) == LOW && ((long)(millis() - endTime) < 0));
  if (((long)(millis() - endTime) >= 0)) {
    operationMode = (operationMode + 1) % 2;
    EEPROM.write(0x0, operationMode);
    digitalWrite(LED, LOW);
    delay(300);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    if (operationMode == LONG) {
      delay(100);
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
    // Wait for the user to release the button and then account for debounce before moving on.
    while (digitalRead(BUTTON) == LOW);
    delay(100);
  } else {
    while (digitalRead(BUTTON) == HIGH);
    for (short i = 255; i > 0; i--) {
      analogWrite(LED, i);
      delay(2);
    }
    digitalWrite(LED, LOW);
  }
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    manualControl();
  }

  lightSensed = (analogRead(CdS) <= threshold);
//  lightSensed = false;

  switch (LEDMode) {
    case OFF:
      if (!lightSensed && !triggered) {
        LEDMode = FADINGON;
        triggered = true;
      } else if (lightSensed && triggered) {
        triggered = false;
      }
      break;
    case FADINGON:
      if (lightSensed) {
        LEDMode = FADINGOFF;
        LEDBrightness = 0;
      } else {
        LEDBrightness++;
        if (LEDBrightness == 255) {
          LEDMode = ON;
          startTime = millis();

          // Checks to see what operating mode the device is in.
          if (operationMode == SHORT) {
            endTime = startTime + SHORT_TIME;
          } else if (operationMode == LONG) {
            endTime = startTime + LONG_TIME;
          } else {
            endTime = startTime + 1;
          }
        }
        analogWrite(LED, LEDBrightness);
        delay(2);
      }
      break;
    case ON:
      // This type-casting math accounts for the 49-day rollover
      if (lightSensed || (long)(millis() - endTime) >= 0) {
        LEDMode = FADINGOFF;
      }
      break;
    case FADINGOFF:
      if (lightSensed) {
        LEDMode = OFF;
        LEDBrightness = 0;
      } else {
        LEDBrightness--;
        if (LEDBrightness == 0) {
          LEDMode = OFF;
        }
        delay(4);
      }
      analogWrite(LED, LEDBrightness);
      break;
  }
}

/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 *            All rights reserved         *
 ******************************************/
#include <EEPROM.h>

// Pin asignments for external parts
#define CdS 2
#define BUTTON 3
#define LED 1

// LED Modes
#define OFF 0
#define FADINGON 1
#define ON 2
#define FADINGOFF 3
#define FASTFADINGOFF 4

// Operation Mode Times (in miliseconds)
// Maximum amount of time is 49-ish days.
unsigned long times[] = {
  15000,   // 15 Seconds
  60000,   // 60 Seconds
  300000   // 5 Minutes
};
// This should match the number of entries in the above table
short numberOfModes = 3;

// How dark it needs to be before triggering the LED (Range: 0 - 1023)
// The higher the number, the darker it needs to be for the LED to turn on.
unsigned int darkThreshold = 750;
unsigned int lightThreshold = 600;

bool lightSensed;
bool darkSensed;
bool triggered;

byte LEDMode;
byte LEDBrightness;
byte operationMode;
unsigned long startTime;
unsigned long endTime;

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
    operationMode = (operationMode + 1) % numberOfModes;
    EEPROM.write(0x0, operationMode);

    digitalWrite(LED, LOW);
    delay(300);
    for (short i; i <= operationMode; i++) {
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(150);
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

void loop() {
  // If the button is pressed, then goto Manual Control mode
  if (digitalRead(BUTTON) == LOW) {
    manualControl();
  }

  // Determines if there is enough light or not
  lightSensed = (analogRead(CdS) <= lightThreshold);
  darkSensed  = (analogRead(CdS) >= darkThreshold);

  switch (LEDMode) {
    case OFF:
      if (darkSensed && !triggered) {
        LEDMode = FADINGON;
        triggered = true;
      } else if (lightSensed && triggered) {
        triggered = false;
      }
      break;
    case FADINGON:
      if (lightSensed) {
        LEDMode = FASTFADINGOFF;
        LEDBrightness = 0;
      } else {
        LEDBrightness++;
        if (LEDBrightness == 255) {
          LEDMode = ON;
          startTime = millis();

          // Checks to see what operating mode the device is in and set the timeout accordingly
          endTime = startTime + times[operationMode];
        }
        analogWrite(LED, LEDBrightness);
        delay(2);
      }
      break;
    case ON:
      // This type-casting math accounts for the 49-day rollover
      if (lightSensed) {
        LEDMode = FASTFADINGOFF;
      } else if ((long)(millis() - endTime) >= 0) {
        LEDMode = FADINGOFF;
      }
      break;
    case FADINGOFF:
      if (lightSensed) {
        LEDMode = FASTFADINGOFF;
      } else {
        LEDBrightness--;
        if (LEDBrightness == 0) {
          LEDMode = OFF;
        }
        delay(4);
      }
      analogWrite(LED, LEDBrightness);
      break;
    case FASTFADINGOFF:
      if (LEDBrightness == 0) {
        LEDMode = OFF;
      } else {
        LEDBrightness--;
        delay(1);
      }
      analogWrite(LED, LEDBrightness);
      break;
  }
}

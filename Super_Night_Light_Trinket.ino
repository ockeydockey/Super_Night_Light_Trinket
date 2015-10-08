/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 ******************************************/
#include <EEPROM.h>

#define CdS 2
#define BUTTON 1
#define KNOB 4
#define KNOBPOWER 3
#define LED 0

// LED Modes
#define OFF 0
#define FADINGON 1
#define ON 2
#define FADINGOFF 3

// Operation Modes
#define SHORT 0
#define LONG 1

unsigned int threshold = 150;

boolean lightSensed;

byte LEDMode;
byte LEDBrightness;
byte operationMode;
unsigned long startTime;
unsigned long endTime;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(CdS, INPUT);
  pinMode(KNOBPOWER, OUTPUT);
  pinMode(KNOB, INPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(KNOBPOWER, HIGH);
  digitalWrite(LED, LOW);

  LEDMode = OFF;
  LEDBrightness = 0;
  startTime = 0;
  endTime = 0;

  // Retreive previous setting, and set to default if no value was set
  EEPROM.get(0x0, operationMode);
  if (operationMode == 255) {
    operationMode = 0;
  }
}

void manualControl() {
  startTime = millis();
  for (byte i = 0; i <= 255; i++) {
    analogWrite(LED, i);
    delay(1);
  }
  endTime = startTime + 3000;  
  while (digitalRead(BUTTON) == LOW && ((long)(millis() - endTime) < 0));
  if (((long)(millis() - endTime) >= 0)) {
    operationMode = (operationMode + 1) % 2;
    EEPROM.update(0x0, operationMode);
    digitalWrite(LED, LOW);
    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    // Wait for the user to release the button and then account for debounce before moving on.
    while (digitalRead(BUTTON) == LOW);
    delay(100);
  } else {
    while (digitalRead(BUTTON) == HIGH);
    for (byte i = 255; i >= 0; i--) {
      analogWrite(LED, i);
      delay(4);
    }
  }
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    manualControl();
  }

  lightSensed = (analogRead(CdS) <= threshold);

  switch (LEDMode) {
    case OFF:
      if (!lightSensed) {
        LEDMode = FADINGON;
      }
      break;
    case FADINGON:
      if (lightSensed) {
        LEDMode = OFF;
        LEDBrightness = 0;
      } else {
        LEDBrightness++;
        if (LEDBrightness == 255) {
          LEDMode = ON;
          startTime = millis();
        }
        analogWrite(LED, LEDBrightness);
        delay(1);
      }
      break;
    case ON:
      // Checks to see what operating mode the device is in.
      // SHORT -> 10 Seconds to 1 Minute
      // LONG  -> 1 Minute to 1 hour
      if (operationMode == SHORT) {
        endTime = startTime + (map(analogRead(KNOB), 0 , 1024 , 10, 60) * 1000);
      } else if (operationMode == LONG) {
        endTime = startTime + (map(analogRead(KNOB), 0 , 1024 , 60, 360) * 1000);
      } else {
        endTime = startTime + 1;
      }
      
      // This type-casting math accounts for the 49-day rollover      
      if ((long)(millis() - endTime) >= 0) {
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
        delay(1);
      }
      analogWrite(LED, LEDBrightness);
      break;
  }
}

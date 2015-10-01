/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 ******************************************/
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
}

void manualControl() {

}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    manualControl();
  }

  lightSensed = (analogRead(CdS) >= threshold);

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
      // See if you can reset the MCU timer so that the 10 day rollover bug never happens
      endTime = startTime + (map(analogRead(KNOB),0 ,1024 , 10, 60) * 1000);
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
      }
      analogWrite(LED, LEDBrightness);
      break;
  }
}

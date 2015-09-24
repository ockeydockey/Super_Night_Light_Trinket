/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 ******************************************/
#define CdS 2
#define buttonPin 1
#define KNOB 4
#define LED 0

unsigned int threshold = 150;
boolean trig = false;
boolean buttonStatus = false;
boolean LEDStatus = false;

boolean finish = false;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(CdS, INPUT);
  pinMode(3, OUTPUT);
  pinMode(KNOB, INPUT);
  pinMode(LED, OUTPUT);
  
  digitalWrite(3, HIGH);
}

void loop() {
  int input = analogRead(CdS);

  if (digitalRead(buttonPin) == HIGH) {
    if (buttonStatus == false) {
      buttonStatus = true;
      LEDStatus = !LEDStatus;
      if (LEDStatus == false) {
        trig = true;
      }
    }
  }
  else {
    buttonStatus = false;
  }

  if (!LEDStatus && input < threshold && !trig) {
    trig = true;

    for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
      // sets the value (range from 0 to 255):
      analogWrite(LED, fadeValue);
      delay(5);  
    }

    boolean cancel = false;
    for (int i = 0; !cancel && i < 5000; i++) {
      analogWrite(LED, 255);
      
      if (digitalRead(buttonPin) == HIGH) {
        cancel = true;
        LEDStatus = false;
        buttonStatus = true;
      }

      delay(1);
    }

    // fade out from max to min
    for(int fadeValue = 255; !cancel && fadeValue >= 0; fadeValue--) { 
      // sets the value (range from 0 to 255):
      analogWrite(LED, fadeValue);

      // wait for 5 milliseconds to see the dimming effect    
      delay(5);
    }
    
    if (cancel) {
      digitalWrite(LED, LOW);
      delay(500);
    }
    
  }
  else if (input >= threshold) {
    trig = false;
  }
}

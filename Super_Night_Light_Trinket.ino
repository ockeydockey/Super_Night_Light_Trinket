/******************************************
 *         "Super Night Light"            *
 *    Code and Hardware designed by:      *
 *            David Ockey                 *
 ******************************************
 * Copyright: David Ockey                 *
 ******************************************/
int R = 255;
int G = 255;
int B = 255;

#define Cds 2;

int threshold = 150;
boolean trig = false;

int buttonPin = 2;
boolean buttonStatus = false;
boolean LEDStatus = false;

boolean finish = false;

// h2rgb() is borrowed from the internet
void h2rgb(float H, int &R, int &G, int &B) {

  int var_i;
  float S=1, V=1, var_1, var_2, var_3, var_h, var_r, var_g, var_b;

  if ( S == 0 )                       //HSV values = 0 ÷ 1
  {
    R = V * 255;
    G = V * 255;
    B = V * 255;
  }
  else
  {
    var_h = H * 6;
    if ( var_h == 6 ) var_h = 0;      //H must be < 1
    var_i = int( var_h ) ;            //Or ... var_i = floor( var_h )
    var_1 = V * ( 1 - S );
    var_2 = V * ( 1 - S * ( var_h - var_i ) );
    var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );

    if      ( var_i == 0 ) {
      var_r = V     ;
      var_g = var_3 ;
      var_b = var_1 ;
    }
    else if ( var_i == 1 ) {
      var_r = var_2 ;
      var_g = V     ;
      var_b = var_1 ;
    }
    else if ( var_i == 2 ) {
      var_r = var_1 ;
      var_g = V     ;
      var_b = var_3 ;
    }
    else if ( var_i == 3 ) {
      var_r = var_1 ;
      var_g = var_2 ;
      var_b = V     ;
    }
    else if ( var_i == 4 ) {
      var_r = var_3 ;
      var_g = var_1 ;
      var_b = V     ;
    }
    else                   {
      var_r = V     ;
      var_g = var_1 ;
      var_b = var_2 ;
    }

    R = (1-var_r) * 255;                  //RGB results = 0 ÷ 255
    G = (1-var_g) * 255;
    B = (1-var_b) * 255;
  }
}

void setup() {
  pinMode(buttonPin, INPUT);
}

void loop() {
  int input = analogRead(Cds);


  h2rgb(h, R, G, B);
  float Rr = (float)R / 255.0;
  float Gr = (float)G / 255.0;
  float Br = (float)B / 255.0;

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

//  analogWrite(RED, (LEDStatus * 255) * Rr);
//  analogWrite(GREEN, (LEDStatus * 255) * Gr);
//  analogWrite(BLUE, (LEDStatus * 255) * Br);

  if (!LEDStatus && input < threshold && !trig) {
    trig = true;

    for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
      // sets the value (range from 0 to 255):
      analogWrite(RED, fadeValue * Rr);
      analogWrite(GREEN, fadeValue * Gr);
      analogWrite(BLUE, fadeValue * Br);
      delay(5);  
    }

    boolean cancel = false;
    for (int i = 0; !cancel && i < 5000; i++) {
      analogWrite(RED, 255 * Rr);
      analogWrite(GREEN, 255 * Gr);
      analogWrite(BLUE, 255 * Br);
      
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
      analogWrite(RED, fadeValue * Rr);
      analogWrite(GREEN, fadeValue * Gr);
      analogWrite(BLUE, fadeValue * Br);

      // wait for 5 milliseconds to see the dimming effect    
      delay(5);  
    }
    
    if (cancel) {
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, LOW);
      delay(500);
    }
    
  }
  else if (input >= threshold) {
    trig = false;
  }
}










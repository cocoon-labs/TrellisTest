#include <Wire.h>
#include "Adafruit_Trellis.h"

#define MOMENTARY 0
#define LATCHING 1
// set the mode here
#define MODE LATCHING 


Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

#define NUMTRELLIS 4

#define numKeys (NUMTRELLIS * 16)

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)
#define INTPIN A2
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line

void setup() {
  Serial.begin(9600);
  Serial.println("Trellis Demo");

  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);
  
  // begin() with the addresses of each panel in order
  trellis.begin(0x70, 0x71, 0x72, 0x73);  // or four!

  // light up all the LEDs in hardware order
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
  
  // light up all the LEDs in morphed order
  for (uint8_t i=0; i<numKeys; i++) {
    int j = morph(i);
    trellis.setLED(j);
    trellis.writeDisplay();    
    delay(50);
  }
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++) {
    int j = morph(i);
    trellis.clrLED(j);
    trellis.writeDisplay();    
    delay(50);
  }
}


void loop() {
  delay(30); // 30ms delay is required, dont remove me!
  
  if (MODE == MOMENTARY) {
    // If a button was just pressed or released...
    if (trellis.readSwitches()) {
      // go through every button
      for (uint8_t i=0; i<numKeys; i++) {
	// if it was pressed, turn it on
	if (trellis.justPressed(i)) {
	  Serial.print("v"); Serial.println(i);
	  trellis.setLED(i);
	} 
	// if it was released, turn it off
	if (trellis.justReleased(i)) {
	  Serial.print("^"); Serial.println(i);
	  trellis.clrLED(i);
	}
      }
      // tell the trellis to set the LEDs we requested
      trellis.writeDisplay();
    }
  }

  if (MODE == LATCHING) {
    // If a button was just pressed or released...
    if (trellis.readSwitches()) {
      // go through every button
      for (uint8_t i=0; i<numKeys; i++) {
        // if it was pressed...
	if (trellis.justPressed(i)) {
	  Serial.print("v"); Serial.println(i);
	  // Alternate the LED
	  if (trellis.isLED(i))
	    trellis.clrLED(i);
	  else
	    trellis.setLED(i);
        } 
      }
      // tell the trellis to set the LEDs we requested
      trellis.writeDisplay();
    }
  }
}

int unmorph(int i) {
  int newI;
  if (i < 16) {
    int r = i / 4;
    newI = i + (r * 4);
  } else if (i < 32) {
    i -= 16;
    int r = i / 4;
    newI = i + 4 + (r * 4);
  } else if (i < 48) {
    i -= 32;
    int r = i / 4;
    newI = i + 36 + (r * 4);
  } else {
    i -= 48;
    int r = i / 4;
    newI = i + 32 + (r * 4);
  }
  return newI;
}

int morph(int i) {
  int newI;
  int r = i / 8;
  int c = i % 8;
  if (i < 32) {
    if (c < 4) {
      newI = (4 * r) + c;
    } else {
      c -= 4;
      newI = (4 * r) + c + 16;
    }    
  } else {
    if (c < 4) {
      r -= 4;
      newI = (4 * r) + c + 48;
    } else {
      r -= 4;
      c -= 4;
      newI = (4 * r) + c + 32;
    }
  }
  return newI;
}

int morphXY(int x, int y) {
  return morph((8 * y) + x);
}

void setLEDxy(int x, int y) {
  trellis.setLED(morphXY(x, y));
}

void clrLEDxy(int x, int y) {
  trellis.clrLED(morphXY(x, y));
}

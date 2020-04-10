#include <Servo.h>
#include "Adafruit_VL53L0X.h"

bool power = true;     // on/off boolean NOTE: in production, set to false. Set to true for debugging.
bool play = false;     // play/pause boolean
int recSquare[10];     // square sound recording
int recTriangle[10];   // triangle sound recording
int ledPin = 0;        // recording light
int ToFLeftPin = A21;  // ToF Left DAC Pin
int ToFRightPin = A22; // ToF Right DAC Pin
int recSize = 0;       // count for number of notes in recording
int DCMotor = 32;      // Pin for DC Motor based on schematic
int rangeMult = 1;     // 1 for about a meter of range playability, 2 for about half a meter, 3 for a third, etc.
Servo laser1;          // laser servo 1
Servo laser2;          // laser servo 2

Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // Left ToF Sensor

// pin setup loop
void setup()   {                
  Serial.begin(115200);

    // wait until serial port opens for native USB devices, remove in final code
  while (! Serial) {
    delay(1);
  }
      Serial.println("GROUP V Loading...");
       if (!lox.begin()) {
       Serial.println(F("Failed to boot the VL53L0X"));
         while(1);
       }
      //Note: default I2C address is 0x29. We can use lox.begin(0x30) to set another address for the second ToF sensor.

  analogWriteResolution(12); // allows for 4092 bits of resolution as opposed to 255. LEDs are set with HIGH which should adjust to 4092.
      
  
  // LOW == off, HIGH == on
  pinMode(33, INPUT_PULLUP);    // power on/off
  pinMode(34, INPUT_PULLUP);    // play
  pinMode(35, INPUT_PULLUP);    // pause
  pinMode(36, INPUT_PULLUP);    // record
  pinMode(A12, INPUT_PULLUP);    // overcurrent resistor
  pinMode(ledPin, OUTPUT);      // recording light
  pinMode(ToFLeftPin, OUTPUT);  // left ToF sensor
  pinMode(ToFRightPin, OUTPUT); // right ToF sensor

  pinMode(DCMotor, OUTPUT);       // DC motor
  
  // Laser servos
  laser1.attach(5);
  laser2.attach(6);
}


// Function returns the data from our ToF Sensor


// Function to change PWM based on desired DC motor speed
void spinPyramid(bool spin, int d) {
  if (spin) {
    analogWrite(DCMotor, HIGH);
    delay(d);
    analogWrite(DCMotor, LOW);
    delay(d);
  }
}


// Function to record sound
void startRecording() {
  // reset saved recording
  recSquare[10] = {};         // warning flagged here; possible error?
  recTriangle[10] = {};       // warning flagged here; possible error?
  recSize = 0;
  
  // wait until record button is pressed again
  while (digitalRead(10) == LOW || recSize < 10) {
    recSquare[recSize] = 1; // TODO: Change to input of tof sensor
    recTriangle[recSize] = 1; // TODO: Change to input of other tof sensor
    recSize ++;
  }
  digitalWrite(ledPin, LOW); // turn recording light off
}


// Function to play recorded sound
void playSound() {

  float angle1, angle2;

  spinPyramid(1, 16);
  for (int i = 0; i < recSize; i++) {

      // Turn on speaker and output recorded sound
      // TODO

      // Laser 1 movement
       angle1 = (recSquare[i] % 90) - 90;
       laser1.write(angle1);

      // Laser 2 movement
       angle2 = (recTriangle[i] % 90) - 90;
       laser2.write(angle2);

       // Check for pause during play function
       if (digitalRead(9) == HIGH) {
         play = false;
       }
  }
  spinPyramid(0, 0);
  play = false; // stop play after goes through recording once
}


// Function to play sound instantly given
// movement over the tof sensors
void normalPlay() {

  int measureL; //intermediate measurment
  float calcL;  //intermediate calculation
  
  VL53L0X_RangingMeasurementData_t measure; // create a measure object for our ToF sensor
  
  Serial.print("Left ToF is measuring... ");
  lox.rangingTest(&measure, false); // false argument just supresses debug info
  
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); 
    measureL = measure.RangeMilliMeter * rangeMult; //adjusts the range to make playing easier
    calcL = (measureL/1200.0)*4092.0; // Range ends around 1100mm, mod as a safeguard, adjust to DAC range
    
    if (calcL>4092){ calcL=4092;}
    Serial.println(calcL); //Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
  
  int tof_left = (int)calcL;  // cast back to an int for the analogWrite
  int tof_right = 0; // TODO: implement right ToF
  
  analogWrite(ToFLeftPin, tof_left); // write value to Left DAC
  // analogWrite(ToFRightPin, tof_right); //write value to Right DAC

  // Change laser direction
  float angle1 = (tof_left % 90) - 90;
  laser1.write(angle1);
  float angle2 = (tof_right % 90) - 90;
  laser2.write(angle2);

  // spins pyramid
  spinPyramid(1, 16);
  delay(100); // for smoothness
}

void loop()                     
{
  if(analogRead(A12)>800){   // Check for overcurrent using pin A12. analogRead returns a number from 0-1023
    power = false;
    break; // exit the loop
    } 
    
  // only use this loop if the power button has been pressed and is on
  while (power == true) {

    // Play sound from recording
    if (play == true) {
      playSound();
    } else {
      normalPlay();
    }

    // Play button
    if (digitalRead(8) == HIGH) {
      play = true;
    }

    // Pause button
    if (digitalRead(9) == HIGH) {
      spinPyramid(0,0);
      play = false;
    }

    // Record button
    if (digitalRead(10) == HIGH) {
      delay(1000);  // wait 1 sec to ensure button is released
      digitalWrite(ledPin, HIGH); // turn recording light on
      startRecording();
      spinPyramid(0,0);
    }

    // Turn power off
    if (digitalRead(7) == HIGH) {
      power = false;
      spinPyramid(0,0);
    }
  }

  // Turn power on
  if (digitalRead(7) == HIGH) {
    power = true;
    spinPyramid(0,0);
  }

}

#include <Servo.h>

bool power = false;   // on/off boolean
bool play = false;    // play/pause boolean
int recSquare[10];    // square sound recording
int recTriangle[10];  // triangle sound recording
int ledPin = 0;       // recording light
int recSize = 0;      // count for number of notes in recording
int DCMotor = 32;     // Pin for DC Motor based on schematic

Servo laser1;         // laser servo 1
Servo laser2;         // laser servo 2

// pin setup loop
void setup()   {                
  Serial.begin(38400);
  
  // LOW == off, HIGH == on
  pinMode(33, INPUT_PULLUP);  // power on/off
  pinMode(34, INPUT_PULLUP);  // play
  pinMode(35, INPUT_PULLUP);  // pause
  pinMode(36, INPUT_PULLUP);  // record
  pinMode(ledPin, OUTPUT);    // recording light

  pinMode(DCMotor, OUTPUT);       // DC motor
  
  // Laser servos
  laser1.attach(5);
  laser2.attach(6);
}


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

  // TODO: read in tof sensors
  int tof_left = 0;  // TODO: change as read-in value
  int tof_right = 0; // TODO: change as read-in value
  
  // TODO: output to speaker

  // Change laser direction
  float angle1 = (tof_left % 90) - 90;
  laser1.write(angle1);
  float angle2 = (tof_right % 90) - 90;
  laser2.write(angle2);

  // spins pyramid
  spinPyramid(1, 16);
  
}

void loop()                     
{
  // only use this loop if the power button has been pressed and is on
  while (power == true) {

    // Play sound from recording
    if (play == true) {
      playSound();
    } else {
      //normalPlay();
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

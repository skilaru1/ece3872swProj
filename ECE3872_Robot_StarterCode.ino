/**
 * This file provides some starting structure for controlling
 * the Elegoo robot car and interfacing with its sensors.
 */
 
// Library for controlling the hobby servo
#include <Servo.h>

// Configuration
// Period of the loop, in ms
#define LoopTime 100

// Motor driver pin connections
#define ENB 5  // left motor enable
#define IN1 7  // left motor control
#define IN2 8  // left motor control
#define ENA 6  // right motor enable
#define IN3 9  // right motor control
#define IN4 11 // right motor control

// Ultrasonic rangefinder pin connections
#define Echo A4
#define Trig A5

// Servo pins
#define ServoPin 3

// Macros to read the three line tracking sensors.
// These will return '0' with a light surface and '1'
// with a dark surface.
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

// global variables
Servo head;  // create servo object to control the looking direction
long prevMillis; // used to time loop()
int count = 0;

/**
 *  Create reusable functions here or in additional files that
 *  you #include.  For example, functions could
 *  set the speed and direction of the left side and right side,
 *  or make the robot as a whole turn left or right or go
 *  straight (or you could do the former and then use that to
 *  create the latter), and functions could interpret sensor
 *  data, change states, enact behaviors, etc.
 */

/**
 * You may or may not want functions like stopRobot().  With your chosen
 * breakdown of functions, is it useful and sensible to have a
 * function that stops the robot?
 */
void stopRobot(){
  // Disable the motors?  Set a speed variable to 0?
  // Depends on your hierarchy and where this function
  // fits into it.
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}



/**
 *  Motor controller control information:
 *  Setting IN1 high and IN2 low sets the left motor forwards.
 *  IN3 low and IN4 high sets the right motor forwards.
 *  Swap those polarities to make the motors turn backwards.
 *  Setting both control signals low applies a brake.
 *  When the IN_ signals are set for forwards or backwards,
 *  the ENA and ENB signals can be modulated with PWM
 *  to control the speed of the motors.  See the analogWrite()
 *  Arduino function for an easy way to create PWM.
 */
 
void forward(int speed) {
  //left motors
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  //right motors
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  //speed control
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void back(int speed) {
  //left motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  //right motors
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  //speed control
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void left(int speed, bool inPlace) {
  //left motors
  digitalWrite(IN1, LOW);
  if (inPlace) {
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN2, LOW);
  }
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  //speed control
  analogWrite(ENA, speed);
}

void right(int speed, bool inPlace) {
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  if(inPlace) {
    digitalWrite(IN3, HIGH);
  } else {
    digitalWrite(IN3, LOW);
  }
  digitalWrite(IN4, LOW);
  
  analogWrite(ENB, speed);
}

/** 
 *  Ultrasonic distance measurement.  Returns distance in cm.
 *  Max distance is ~1.5m, based on the timeout of pulseIn().
 *  This is an example of a lowest-level function, since it
 *  interacts with, and depends on, the lowest level of hardware
 *  of the processor and sensor.
 */
int readDistance() {
  digitalWrite(Trig, LOW);  // ensure ping is off
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH); // start ping
  delayMicroseconds(10); 
  digitalWrite(Trig, LOW);  // end ping
  return (int)pulseIn(Echo, HIGH, 10000) / 58; // measure time of flight and convert to cm
}

/**
 * waitForTick() blocks until a periodic time based on the global millis() time,
 * which can be used to force loop() to run at a predictable rate (as long as
 * the code in loop() is faster than the tick time).
 * This may or may not be desirable for your code, but it usually is.
 */
void waitForTick(){
  // block until the specified time
  while((millis() - prevMillis) <= LoopTime);
  prevMillis = millis();
  return;
}


void setup(){
  // Start serial comm in case you want to debug with it
  Serial.begin(9600);

  // Configure the pins that are outputs
  pinMode(Trig, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(ServoPin, OUTPUT);

  // Attach the servo controller to the servo pin
  head.attach(ServoPin);
  // Set the head looking forward.
  head.write(90);

  // record the current time for the timing function
  prevMillis = millis();

  
}

void loop() {
  // calling waitForTick() at the beginning of loop will keep it periodic
  waitForTick(); 
  
  // Example of reading the ultrasonic rangefinder and printing to
  // the serial port.
  // Note that readDistance() is blocking, meaning that it will prevent
  // any other code from executing until it returns.  This will
  // take a variable amount of time, up to ~10 ms.
  Serial.println(readDistance());
//  if (count < 50) forward(127);
//  else if (count < 100) back(127);
  if (count < 50) right(255, true);
  else if (count < 100) left(255, true);
  else count = 0;

  ++count;
  
  // Example of how the sensor macros can be used.  Whether or not this
  // type of sensor interaction belongs in loop() is up to your
  // code structure.
  if(LT_M){
    // Do something based on the middle sensor detecting a dark surface
  }

}

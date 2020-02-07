/**
   This file provides some starting structure for controlling
   the Elegoo robot car and interfacing with its sensors.
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
int state = 0;  // current state (A = 0, B = 1, etc.)
int curr = 0;   // current distance read
int turn;   // 0 for left, 1 for right
boolean startTurn = false; // when to turn at top of loop
int prev;
boolean seenBlock = false;
int turnTimer = 0;
boolean stateF = false;
/**
    Create reusable functions here or in additional files that
    you #include.  For example, functions could
    set the speed and direction of the left side and right side,
    or make the robot as a whole turn left or right or go
    straight (or you could do the former and then use that to
    create the latter), and functions could interpret sensor
    data, change states, enact behaviors, etc.
*/

/**
   You may or may not want functions like stopRobot().  With your chosen
   breakdown of functions, is it useful and sensible to have a
   function that stops the robot?
*/
void stopRobot() {
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
    Motor controller control information:
    Setting IN1 high and IN2 low sets the left motor forwards.
    IN3 low and IN4 high sets the right motor forwards.
    Swap those polarities to make the motors turn backwards.
    Setting both control signals low applies a brake.
    When the IN_ signals are set for forwards or backwards,
    the ENA and ENB signals can be modulated with PWM
    to control the speed of the motors.  See the analogWrite()
    Arduino function for an easy way to create PWM.
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

  if (inPlace) {
    digitalWrite(IN3, HIGH);
  } else {
    digitalWrite(IN3, LOW);
  }
  digitalWrite(IN4, LOW);

  analogWrite(ENB, speed);
}

void leftInPlace(int speed) {
  //left motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  //speed control
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void rightInPlace(int speed) {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}
/**
    Ultrasonic distance measurement.  Returns distance in cm.
    Max distance is ~1.5m, based on the timeout of pulseIn().
    This is an example of a lowest-level function, since it
    interacts with, and depends on, the lowest level of hardware
    of the processor and sensor.
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
   waitForTick() blocks until a periodic time based on the global millis() time,
   which can be used to force loop() to run at a predictable rate (as long as
   the code in loop() is faster than the tick time).
   This may or may not be desirable for your code, but it usually is.
*/
void waitForTick() {
  // block until the specified time
  while ((millis() - prevMillis) <= LoopTime);
  prevMillis = millis();
  return;
}


void setup() {
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


void moveOnLine() {

  if (LT_R) {
    right(225, true);
    prev = 1;
  } else if (LT_L) {
    left (225, true);
    prev = 0;
  } else if (LT_M || (LT_R && LT_L && LT_M)) {
    forward(125);
  } else {
    if (prev == 1) {
      rightInPlace(150);
    } else {
      leftInPlace(150);
    }
  }
  return;
}

void newMoveOnLine() {

  if (LT_M || (LT_R && LT_L && LT_M)) {
    forward(125);
  } else if (LT_L) {
    leftInPlace (150);
    prev = 0;
  } else if (LT_R) {
    rightInPlace(150);
    prev = 1;
  } else {
    if (prev == 1 || turn == 1) {
      rightInPlace(150);
    } else if (prev == 0 || turn == 0) {
      leftInPlace(150);
    } else {
      forward(125);
    }
  }
  return;
}

void state0MoveOnLine() {

  //  if (LT_M || (LT_R && LT_L && LT_M)) {
  //    forward(125);
  //  } else if (LT_R) {
  //    right(225, true);
  //    prev = 1;
  //  } else if (LT_L) {
  //    left (225, true);
  //    prev = 0;
  //  }  else {
  //    if (prev == 1) {
  //      rightInPlace(150);
  //    } else {
  //      leftInPlace(150);
  //    }
  //  }
  //  return;

  if (LT_M) {
    forward(125);
  } else if (LT_R) {
    rightInPlace(150);
    prev = 1;
  } else if (LT_L) {
    leftInPlace(150);
    prev = 0;
  } else {
    if (prev == 0) {
      leftInPlace(150);
    } else {
      rightInPlace(150);
    }
  }
}
void loop() {
  // calling waitForTick() at the beginning of loop will keep it periodic
  waitForTick();

  // Iterate through state machine
  // STATE: A
  if (state == 0) {         // A: start state
    stateF = false;
    Serial.println("State A");
    state0MoveOnLine();

    curr = readDistance();
    Serial.println(curr);
    if (curr <= 10 && curr > 0) {
      stopRobot();
      state = 1;      // if close to block A, move to state B
      head.write(180);     // turn sonar head to left
      delay(1000);
    }

    // STATE: B
  } else if (state == 1) {  // B: check for block B state
    Serial.println("State B");

    curr = readDistance();
    Serial.println(curr);
    if (curr <= 20 && curr > 0) {
      turn = 0;      // block B found, move to the left with state C
      Serial.println("Turn Left");
    } else {
      turn = 1;      // block B not found, move to right with state D
      Serial.println("Turn Right");
    }

    state = 2;
    head.write(90);   // turn sonar head forward
    delay(1000);

    // STATE: C & D
  } else if (state == 2) {  // C and D: wait for A
    Serial.println("State C and D (wait for A)");

    curr = readDistance();
    Serial.println(curr);
    if (curr > 20) {   // check to see if block A has moved
      state = 3;
      head.write(0);  // turn head to right for block C
      delay(1000);
    }

    // STATE: C & D MOVE
  } else if (state == 3) {  // C and D: move to the right or left depending on "turn" value
    Serial.println("State C and D (start turn)");

    if (LT_L || LT_R || LT_M) {
      if (turn == 0) {
        left(250, true);
        prev = 0;
      } else {
        right(250, true);
        prev = 1;
      }
    } else {
      stopRobot();
      delay(2000);
      Serial.println("GO TO 6");
      state = 6;
    }

    // part 2 of State C/D
  } else if (state == 6) {
    if (!LT_L && !LT_M && !LT_R) {
      Serial.println("NONE ON");
      forward(100);
    } else {
      Serial.println("FOUND LINE");
      state = 4;
    }
    // STATE: E
  } else if (state == 4) {  // E: follow line and check for block C and black bar, return to state A when found
    Serial.println("State E");
    //moveOnLine();
    newMoveOnLine();


    if (readDistance() > 0)
      curr = readDistance();
    Serial.println(curr);
    if (curr < 20 && stateF == false) {   // check for block C existance
      state = 5;
      seenBlock = true;
    }
    if (LT_M && LT_R && LT_L && seenBlock == true) {   // check for long black bar
      stopRobot();
      state = 7;
      seenBlock = false;
    }
  } else if (state == 7) {
    turnTimer++;
    if (turnTimer < 5) {
      if (turn == 0) {
        leftInPlace(150);
      } else {
        rightInPlace(150);
      }
    } else if (turnTimer < 7) {
      forward(100);
    } else {
      turnTimer = 0;
      stopRobot();
      head.write(90);
      delay(2000);
      state = 0;
    }
    // STATE: F
  } else if (state == 5) {  // F: if block C found, wait then return to E state
    turnTimer++;
    Serial.println("State F");
    stopRobot();
    delay(500);

    if (readDistance() > 0)
      curr = readDistance();
    Serial.println(curr);
    Serial.print("TT: ");
    Serial.println(turnTimer);
    if (curr > 20  || curr == 0) {
      turnTimer = 0;
      state = 4;
    }

  }

}

#include <Arduino.h>
#include <Romi32U4.h>
#include <Chassis.h>
#include <servo32u4.h>
#include <BlueMotor.h>

#define COEFF 11.111111

/*
SOME USEFUL VALUES

Grabber closed: 1700us
Grabber open: 1300us

Servo 90deg: 1750
Servo 135deg: 1250

So 11.1111 per 1deg

Blue motor: approx. 1600 per 90deg

So 17.7778 per 1deg

BLUE ENCODER GOES DOWN AS BLUE EFFORT GOES UP

POSITIONS

top level: blue pos: 2288; servo: 1550
mid level: blue pos: 2288; servo: 1850
low level: blue pos: -854; servo: 1800

*/

Chassis chassis;
Romi32U4ButtonB buttonB;

// DT Control variables
int leftEffort = 0; // Effort of left DT motor
int rightEffort = 0; // Effort of right DT motor
int dtIncrement = 20; // Effort

// Blue motor control variables
BlueMotor blueMotor; // The Blue Motor object
int blueEffort = 0; // Effort to send to the blue motor (only runs above approximately +/-150 effort)
int blueIncrement = 20; // The amount to increment the blue motor effort by when requested

// Servo control variables
Servo32U4Pin5 servo1;
Servo32U4Pin12 servo2;
int servo2Pos = 1500; // Position of servo 2 (microseconds)
int servoIncrement = 50; // The amount to increment the servo position by (microseconds)

char lastInput;

float px = 100;
float py = 90;

// Lengths (mm)
int l1 = 100;
int l2 = 120;
int l3 = 95;
int l4 = 95;
int l5 = 95;

// Global counter
int index_height = 0;

float degToTicks(float deg) {
  return (deg-75)*18;
}

float degToMs(float deg) {
  return 2600-deg*COEFF;
}

float getTheta4(float px, float py) {
    // Calculate required lengths and angles
    float PC = sqrt((l1 - px)*(l1 - px) + py*py);
    float alpha = PI - atan(py / (l1 - px));
    float beta = acos((l4*l4 - l5*l5 - PC*PC) / (-2 * l5 * PC));

    return (alpha - beta) * 180 / PI; 
}

float getTheta2(float px, float py) {
    // Calculate required lengths and angles
    float OP = sqrt(px*px + py*py);
    float gamma = atan(py / px);
    float delta = acos((l3*l3 - l2*l2 - OP*OP) / (-2 * l2 * OP));

    return 180 * (gamma + delta) / PI;
}

float theta2start = getTheta2(px, py);
float theta4start = getTheta4(px, py);

float blueTarget = degToTicks(theta4start);
float servo1Pos = 1750;

void runBlueMotor() {
  if (blueMotor.getPosition() < blueTarget) {
      blueEffort = -300;
      blueMotor.setEffort(blueEffort); // Update blue motor effort. Will not run at less than ~150 effort magnitude
    }
    if (blueMotor.getPosition() > blueTarget) {
      blueEffort = 300;
      blueMotor.setEffort(blueEffort); // Update blue motor effort. Will not run at less than ~150 effort magnitude
    }
}

void setup() {
  Serial.begin(9600);
  delay(500); // Pause to allow internal operations

  chassis.init();
  blueMotor.setup();
 
  // Servo Setup
  servo1.attach();
  servo2.attach();
  servo1.writeMicroseconds(servo1Pos);
  servo2.writeMicroseconds(servo2Pos);

  delay(500); // Pause to ensure all previous processes have completed
  Serial.println("Setup Completed");
}

void loop() {
  // Keyboard controls
  if (Serial.available()) { // If a command is waiting
    char input = Serial.read();
    lastInput = input;

    if (input == 'w') { // Move forward
      leftEffort += dtIncrement;
      rightEffort += dtIncrement;
    } 
    else if (input == 'a') { // Turn left
      leftEffort -= dtIncrement;
      rightEffort += dtIncrement;
    } 
    else if (input == 's') { // Move backward
      leftEffort -= dtIncrement;
      rightEffort -= dtIncrement;
    } 
    else if (input == 'd') { // Turn right
      leftEffort += dtIncrement;
      rightEffort -= dtIncrement;
    } /*
    else if (input == 'm') { // Blue motor speed up
      blueEffort += blueIncrement;
    } 
    else if (input == 'n') { //  Blue motor speed down
      blueEffort -= blueIncrement;
    } 
    else if (input == '1') { // Servo one increase position
      servo1Pos += servoIncrement;
      servo1.writeMicroseconds(servo1Pos);
    } 
    else if (input == '2') { // Servo one decrease position
      servo1Pos -= servoIncrement;
      servo1.writeMicroseconds(servo1Pos);
    } 
    else if (input == '3') { // Servo two increase position
      servo2Pos += servoIncrement;
      servo2.writeMicroseconds(servo2Pos);
    } 
    else if (input == '4') { // Servo two decrease position
      servo2Pos -= servoIncrement;
      servo2.writeMicroseconds(servo2Pos);
    } */
    else if (input == '{') { // Stop the motors if "{" is pressed
      leftEffort = rightEffort = 0;
      blueEffort = 0;
    }

    //CONTROLS
    else if (input == 'c') { // Close grabber
      servo2.writeMicroseconds(1700);
    } 

    else if (input == 'o') { // Open grabber
      servo2.writeMicroseconds(1200);
    } 

    else if (input == '3') { // Go up
      py += 5;
      Serial.println("Theta2: " + String(getTheta2(px,py)));
      Serial.println("Theta4: " + String(getTheta4(px,py)));
      Serial.println(py);
      blueTarget = degToTicks(getTheta4(px,py));
      servo1Pos = degToMs(getTheta2(px,py));
    }

    else if (input == '1') { // Go down
      py -= 5;
      Serial.println("Theta2: " + String(getTheta2(px,py)));
      Serial.println("Theta4: " + String(getTheta4(px,py)));
      Serial.println(py);
      blueTarget = degToTicks(getTheta4(px,py));
      servo1Pos = degToMs(getTheta2(px,py));
    }

    else if (input == '4') { // Go x+
      px += 5;
      Serial.println("Theta2: " + String(getTheta2(px,py)));
      Serial.println("Theta4: " + String(getTheta4(px,py)));
      Serial.println(px);
      blueTarget = degToTicks(getTheta4(px,py));
      servo1Pos = degToMs(getTheta2(px,py));
    }

    else if (input == '5') { // Go x-
      px -= 5;
      Serial.println("Theta2: " + String(getTheta2(px,py)));
      Serial.println("Theta4: " + String(getTheta4(px,py)));
      Serial.println(px);
      blueTarget = degToTicks(getTheta4(px,py));
      servo1Pos = degToMs(getTheta2(px,py));
    }

    // px, py absolute coordinates
    // give to theta 2 and 4
    // 

  
    chassis.setMotorEfforts(leftEffort, rightEffort); // Update wheel motor efforts
  }
    servo1.writeMicroseconds(servo1Pos);
    runBlueMotor();
  // Serial.println("Blue Motor Pos: " + String(blueMotor.getPosition()));
  // Serial.println("Romi Pos: " + String(chassis.getLeftEncoderCount()) + ", " + String(chassis.getRightEncoderCount()));
  // Serial.println("Blue Effort: " + String(blueEffort));
  // Serial.println("Grabber: " + String(servo2Pos) + "; blue effort: " + String(blueEffort) + "; blue pos: " + String(blueMotor.getPosition()) + "; servo: " + String(servo1Pos));
  // Serial.println("Blue target: " + String(blueTarget) + "; blue effort: " + String(blueEffort) + "; blue pos: " + String(blueMotor.getPosition()));

}

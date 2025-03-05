#include <Arduino.h>
#include <Romi32U4.h>
#include <Chassis.h>
#include <servo32u4.h>
#include <BlueMotor.h>

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
int servo1Pos = 1500; // Position of servo 1 (microseconds)
int servo2Pos = 1500; // Position of servo 2 (microseconds)
int servoIncrement = 50; // The amount to increment the servo position by (microseconds)

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
    } 
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
    } 
    else if (input == '{') { // Stop the motors if "{" is pressed
      leftEffort = rightEffort = 0;
      blueEffort = 0;
    }

  
    chassis.setMotorEfforts(leftEffort, rightEffort); // Update wheel motor efforts
    blueMotor.setEffort(blueEffort); // Update blue motor effort. Will not run at less than ~150 effort magnitude
  }

  // Serial.println("Blue Motor Pos: " + String(blueMotor.getPosition()));
  // Serial.println("Romi Pos: " + String(chassis.getLeftEncoderCount()) + ", " + String(chassis.getRightEncoderCount()));
  // Serial.println("Blue Effort: " + String(blueEffort));
  Serial.println(String(servo2Pos) + ", " + String(blueEffort));
}

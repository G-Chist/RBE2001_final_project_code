#include <Romi32U4.h>
#include <Servo.h> 

#define ServoPin 12

Servo servo;

Romi32U4Encoders encoders;
Romi32U4LCD lcd;
Romi32U4Buzzer buzzer;
Romi32U4Motors motors;
Romi32U4ButtonA buttonA;
Romi32U4ButtonC buttonC;

int val;
int command = 0;
int speed_right = 0;
int speed_left = 0;
int target_right = 0;
int target_left = 0;
int ticksPerCm = 59; // Approximate number of ticks per cm 
int ticksPerDegree = 780/90; // Approximate number of ticks per 1 degree of rotation around the Romi's axis
String lastCommand = "";
float Kp = 0.7; // Proportional coefficient
long count = 0;
unsigned time = 0;

void isr() {
  if (digitalRead(1)) {
    count++;
  } else {
    count--;
  }
} 

void setup() {
  // put your setup code here, to run once:
  servo.attach(ServoPin);
  Serial.begin(9600);
}

void loop() {

    if (val == 'w') {
      Serial.println("Forward 1cm");
      target_right += ticksPerCm*1;
      target_left += ticksPerCm*1;
      lastCommand = "w";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == '2') {
      Serial.println("Forward 5cm");
      target_right += ticksPerCm*5;
      target_left += ticksPerCm*5;
      lastCommand = "2";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == 'a') {
      Serial.println("10 degrees CCW");
      target_right -= ticksPerDegree*10;
      target_left += ticksPerDegree*10;
      lastCommand = "a";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == 'd') {
      Serial.println("10 degrees CW");
      target_right += ticksPerDegree*10;
      target_left -= ticksPerDegree*10;
      lastCommand = "d";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == 's') {
      Serial.println("Back 1cm");
      target_right -= ticksPerCm*1;
      target_left -= ticksPerCm*1;
      lastCommand = "s";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == 'x') {
      Serial.println("Back 5cm");
      target_right -= ticksPerCm*5;
      target_left -= ticksPerCm*5;
      lastCommand = "x";
      Serial.print(target_right);
      Serial.print(", ");
      Serial.println(target_left);
    }

    if (val == 'g') {
      Serial.println("Servo");
      lastCommand = "g";
      servo.write(0);
    }

    if (!(lastCommand == "a" || lastCommand == "d")) {
      speed_right = -Kp*(encoders.getCountsRight()-target_right);
      speed_left = -Kp*(encoders.getCountsRight()-target_left);
    } else {
      speed_right = Kp*(encoders.getCountsRight()-target_right);
      speed_left = -Kp*(encoders.getCountsRight()-target_right);
    }



    if (speed_right > 100) {speed_right = 100;}
    if (speed_right < -100) {speed_right = -100;}

    if (speed_left > 100) {speed_left = 100;}
    if (speed_left < -100) {speed_left = -100;}

    motors.setSpeeds(speed_right, speed_left);

    val = Serial.read();

    if (fmod(floor(millis()/1000),2) == 0) {
      // Turn the LEDs on.
      ledRed(1);
      ledYellow(1);
      ledGreen(1);
      // Serial.println("Blink ON");
    }

    if (fmod(floor(millis()/1000),2) == 1) {
      // Turn the LEDs off.
      ledRed(0);
      ledYellow(0);
      ledGreen(0);
      // Serial.println("Blink OFF");
    }
}

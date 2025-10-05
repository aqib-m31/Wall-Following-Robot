/*
  WallFollowingRobot.ino
  A autonomous right-wall following robot using three ultrasonic sensors (front, left, and right).
  The robot moves forward while maintaining a safe distance from the right wall, adjusting its path
  if the wall is too close or too far (bang-bang control).
*/

#include <NewPing.h>

// Motor Pins
#define PWMA 6   // Left Motor Speed pin (ENA)
#define AIN2 A0  // Motor-L forward (IN2).
#define AIN1 A1  // Motor-L backward (IN1)
#define PWMB 5   // Right Motor Speed pin (ENB)
#define BIN1 A2  // Motor-R forward (IN3)
#define BIN2 A3  // Motor-R backward (IN4)

// Ultrasonic Sensor Configuration
#define ECHO_FRONT 2
#define TRIG_FRONT 3
#define ECHO_LEFT 8
#define TRIG_LEFT 7
#define ECHO_RIGHT 10
#define TRIG_RIGHT 9

const int MAX_DISTANCE = 400;
NewPing sonarFront(TRIG_FRONT, ECHO_FRONT, MAX_DISTANCE);
NewPing sonarLeft(TRIG_LEFT, ECHO_LEFT, MAX_DISTANCE);
NewPing sonarRight(TRIG_RIGHT, ECHO_RIGHT, MAX_DISTANCE);

// Distance Variables
int distanceFront = 0;
int distanceLeft = 0;
int distanceRight = 0;
int safe_distance_front = 15;
int safe_distance_right = 8;
int safe_distance_left = 10;

// Move robot backward
void reverse(int speed = 50) {
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

// Turn slightly right (while moving forward)
void turnRight(int speed = 40) {
  Serial.println("Turning Right");
  analogWrite(PWMA, 50);  // Left motor speed (slightly faster)
  analogWrite(PWMB, speed);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

// Turn slightly left (while moving forward)
void turnLeft(int speed = 40) {
  Serial.println("Turning Left");
  analogWrite(PWMA, speed);
  analogWrite(PWMB, 50);  // Right motor speed (slightly faster)
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

// Stop both motors for a given duration
void stop(int time = 0) {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(time);
}

// Spin left in place (one wheel forward, one backward)
void turnLeftInPlace(int speed = 50) {
  Serial.println("Turning Left in place");
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

// Spin right in place (one wheel forward, one backward)
void turnRightInPlace(int speed = 50) {
  Serial.println("Turning Right in place");
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

// Move forward, allowing different speeds for each wheel (for steering)
void forward(int speedLeft = 45, int speedRight = 45) {
  analogWrite(PWMA, speedLeft);
  analogWrite(PWMB, speedRight);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void setup() {
  Serial.begin(9600);
  // Set ultrasonic pins as input/output
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_FRONT, OUTPUT);

  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);

  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);

  // Set motor pins as outputs
  pinMode(PWMA, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  // Stop motors at start
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void loop() {
  // Read distance values (in cm) from each ultrasonic sensor
  distanceFront = sonarFront.ping_cm();
  distanceLeft = sonarLeft.ping_cm();
  distanceRight = sonarRight.ping_cm();

  // Print sensor readings for debugging
  Serial.print("Left: ");
  Serial.println(distanceLeft);
  Serial.print("Front: ");
  Serial.println(distanceFront);
  Serial.print("Right: ");
  Serial.println(distanceRight);
  Serial.println();

  // If no wall detected on the right steer right (L - outside turn)
  if ((distanceRight == 0) || distanceRight > 50) {
    forward(100, 40);  // Turn right
  } else if ((distanceFront < safe_distance_front) && (distanceFront != 0)) {
    // Wall at front (⌝)
    turnLeftInPlace();
  } else if (distanceRight < safe_distance_right - 1) {
    // If too close to right wall, steer left
    turnLeft();
  } else if (distanceRight > safe_distance_right + 1) {
    // If too far from right wall, steer right
    turnRight();
  } else {
    // Go straight by default
    forward();
  }
}
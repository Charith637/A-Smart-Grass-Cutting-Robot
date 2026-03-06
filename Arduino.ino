#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ------------------- Motor Pins -------------------
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

// ------------------- Relay Pin -------------------
int BLADE_RELAY_PIN = 2;

// ------------------- Ultrasonic Pins -------------------
#define trigPin 9
#define echoPin 8

// ------------------- GPS -------------------
SoftwareSerial gps(11, 12); // RX, TX

// ------------------- LCD -------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- GRID SETTINGS -------------------
#define ROWS 3
#define COLS 4
#define CELL_TIME 2000
#define TURN_TIME 1300
#define REVERSE_TIME 800
#define OBSTACLE_DIST 20

int grid[ROWS][COLS] = {
  {0,0,0,0},
  {0,1,0,0},
  {0,0,0,0}
};

// ------------------- SETUP -------------------
void setup() {

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(BLADE_RELAY_PIN, OUTPUT);
  digitalWrite(BLADE_RELAY_PIN, LOW); // blade ON

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  gps.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("CPP Robot Start");
  delay(2000);
  lcd.clear();
}

// ------------------- MAIN LOOP -------------------
void loop() {

  zigzagCoverage();

  stopMotors();
  digitalWrite(BLADE_RELAY_PIN, HIGH); // blade OFF

  lcd.clear();
  lcd.print("Coverage Done");

  while(1);
}

// ------------------- CPP LOGIC -------------------
void zigzagCoverage() {

  for (int i = 0; i < ROWS; i++) {

    if (i % 2 == 0) {
      for (int j = 0; j < COLS; j++) {
        visitCell(i, j);
      }
    }
    else {
      for (int j = COLS - 1; j >= 0; j--) {
        visitCell(i, j);
      }
    }

    if (i < ROWS - 1) {
      moveToNextRow(i);
    }
  }
}

void visitCell(int r, int c) {

  if (grid[r][c] == 1) return;

  if (checkObstacle()) {

    lcd.setCursor(0,1);
    lcd.print("Obstacle!     ");

    stopMotors();
    delay(400);

    // Reverse a bit
    moveBackward();
    delay(REVERSE_TIME);
    stopMotors();
    delay(400);

    // Pivot right
    turnRight();
    delay(TURN_TIME);
    stopMotors();
    delay(400);

    return;
  }

  lcd.setCursor(0,0);
  lcd.print("Row:");
  lcd.print(r);
  lcd.print(" Col:");
  lcd.print(c);

  moveForward();
  delay(CELL_TIME);
  stopMotors();
  delay(300);

  grid[r][c] = 2;
}

// ------------------- MOVE TO NEXT ROW -------------------
void moveToNextRow(int row) {

  if (row % 2 == 0) {

    turnRight();
    delay(TURN_TIME);
    stopMotors();

    moveForwardCell();

    turnRight();
    delay(TURN_TIME);
    stopMotors();
  }
  else {

    turnLeft();
    delay(TURN_TIME);
    stopMotors();

    moveForwardCell();

    turnLeft();
    delay(TURN_TIME);
    stopMotors();
  }
}

void moveForwardCell() {
  moveForward();
  delay(CELL_TIME);
  stopMotors();
  delay(300);
}

// ------------------- OBSTACLE CHECK -------------------
bool checkObstacle() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance;

  if (duration == 0) distance = 999;
  else distance = duration * 0.034 / 2;

  lcd.setCursor(0,1);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm   ");

  if (distance < OBSTACLE_DIST) return true;
  return false;
}

// ------------------- MOTOR FUNCTIONS -------------------

void moveForward() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveBackward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Pivot turn (stronger for heavy robot)
void turnRight() {

  // left wheel forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // right wheel stop
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void turnLeft() {

  // left wheel stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // right wheel forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
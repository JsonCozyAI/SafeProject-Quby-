#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// -------------------- Tastatur Setup --------------------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// -------------------- LCD & Servo --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;
const int servoPin = 10;
const int SERVO_ON_ANGLE = 90;
const int SERVO_OFF_ANGLE = 0;

// -------------------- LED & Sensor --------------------
const int sensorPin = A0;
const int greenLedPin = 13;
const int lightThreshold = 950;

// -------------------- Quiz --------------------
int a, b, correctAnswer;
String inputAnswer = "";
bool vaultOpen = false;

void generateQuiz() {
  a = random(1, 20);
  b = random(1, 20);
  correctAnswer = a + b;
  inputAnswer = "";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Quiz: ");
  lcd.print(a);
  lcd.print(" + ");
  lcd.print(b);
  lcd.setCursor(0, 1);
  lcd.print("Answer: ");
}

void setup() {
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Math Quiz Lock");
  delay(2000);

  myServo.attach(servoPin);
  myServo.write(SERVO_OFF_ANGLE);

  pinMode(greenLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);

  generateQuiz();

  Serial.begin(9600);
}

void loop() {
  if (vaultOpen) {
    int sensorValue = analogRead(sensorPin);
    Serial.println(sensorValue);
    if (sensorValue > lightThreshold) {
      digitalWrite(greenLedPin, HIGH);
    } else {
      digitalWrite(greenLedPin, LOW);
    }
    delay(100);
  }

  char key = keypad.getKey();
  if (key) {
    if (key >= '0' && key <= '9') {
      inputAnswer += key;
      lcd.setCursor(9, 1);
      lcd.print(inputAnswer);
    }

    else if (key == 'B') {
      inputAnswer = "";
      lcd.setCursor(9, 1);
      lcd.print("    ");
      lcd.setCursor(9, 1);
    }

    else if (key == 'A') {
      if (inputAnswer.toInt() == correctAnswer) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Correct!");
        lcd.setCursor(0, 1);
        lcd.print("Vault is open");

        myServo.write(SERVO_ON_ANGLE);
        vaultOpen = true;
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong answer");
        lcd.setCursor(0, 1);
        lcd.print("Try again!");
        delay(1500);
        generateQuiz();
      }
    }

    else if (key == 'D' && vaultOpen) {
      myServo.write(SERVO_OFF_ANGLE);
      digitalWrite(greenLedPin, LOW);
      vaultOpen = false;
      generateQuiz();
    }
  }
}

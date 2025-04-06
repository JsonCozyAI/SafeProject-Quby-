#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Tastenfeld-Konfiguration (4x4)
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // A: Code überprüfen
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }   // D: Servo zurücksetzen (Tresor schließen)
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD (I2C)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo
Servo myServo;
const int servoPin = 10;
const int SERVO_ON_ANGLE = 90;
const int SERVO_OFF_ANGLE = 0;

// Zahlencode
String secretCode = "";
String userInput = "";

// Lichtsensor & LED
const int sensorPin = A0;
const int ledPin = 13;
int threshold = 865;
bool vaultOpen = false;

// Zufälliger 4-stelliger Code
void generateSecretCode() {
  secretCode = "";
  int digits[10] = {0,1,2,3,4,5,6,7,8,9};
  for (int i = 9; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = digits[i];
    digits[i] = digits[j];
    digits[j] = temp;
  }
  for (int i = 0; i < 4; i++) {
    secretCode += String(digits[i]);
  }
}

void setup() {
  randomSeed(analogRead(0));
  generateSecretCode();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Number");
  lcd.setCursor(0,1);
  lcd.print("Baseball");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Vault Code");

  myServo.attach(servoPin);
  myServo.write(SERVO_OFF_ANGLE);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
}

void loop() {
  if (vaultOpen) {
    int sensorValue = analogRead(sensorPin);
    Serial.print("Brightness: ");
    Serial.println(sensorValue);
    if(sensorValue >= threshold) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
    delay(100);
  }

  char key = keypad.getKey();
  if (key) {
    if (key >= '0' && key <= '9') {
      if (userInput.length() < 4) userInput += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input: " + userInput);
    }
    else if (key == 'A') {
      if (userInput.length() != 4) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Need 4 digits!");
        delay(1500);
      } else {
        int strikes = 0, balls = 0;
        for (int i = 0; i < 4; i++) {
          if (userInput[i] == secretCode[i]) strikes++;
          else if (secretCode.indexOf(userInput[i]) != -1) balls++;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        if (strikes == 4) {
          lcd.print("Vault Open!");
          myServo.write(SERVO_ON_ANGLE);
          vaultOpen = true;
        } else if (strikes == 0 && balls == 0) {
          lcd.print("Out");
        } else {
          lcd.print("S:" + String(strikes) + " B:" + String(balls));
        }
        delay(2000);
      }
      userInput = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Vault Code");
    }
    else if (key == 'D') {
      myServo.write(SERVO_OFF_ANGLE);
      digitalWrite(ledPin, LOW);
      vaultOpen = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vault Closed!");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Vault Code");
      userInput = "";
    }
    else if (key == '*') {
      userInput = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cleared input");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Vault Code");
    }
  }
}

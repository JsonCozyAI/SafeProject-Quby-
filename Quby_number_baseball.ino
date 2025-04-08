#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

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
String secretCode = "";
String userInput = "";

// -------------------- Sensor & LED --------------------
const int sensorPin = A0;    // Lichtsensor
const int greenLedPin = 13;  // LED zeigt Lichtniveau
const int lightThreshold = 960;
bool vaultOpen = false;

// -------------------- NeoPixel --------------------
#define NUMPIXELS 80
#define LED_PIN 11
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_RGBW + NEO_KHZ800);

// -------------------- Funktionen --------------------
void showBlueIdle() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255)); // Blau
  }
  strip.show();
}

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
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);

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

  strip.begin();
  strip.setBrightness(50);
  strip.show();

  Serial.begin(9600);
}

void loop() {
  if (!vaultOpen) {
    showBlueIdle(); // Normalzustand
  } else {
    int sensorValue = analogRead(sensorPin);
    Serial.print("Brightness: ");
    Serial.println(sensorValue);
    if (sensorValue >= lightThreshold) {
      digitalWrite(greenLedPin, HIGH); // LED bei Licht
    } else {
      digitalWrite(greenLedPin, LOW);
    }
    delay(100);
  }

  char key = keypad.getKey();
  if (key) {
    if (key >= '0' && key <= '9') {
      if (userInput.length() < 4) {
        if (userInput.indexOf(key) != -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Duplicate digit!");
          delay(1500);
          userInput = "";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Vault Code");
        } else {
          userInput += key;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Input: " + userInput);
        }
      }
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
          lcd.print("Congratulations!");
          delay(1500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vault is now open");
          myServo.write(SERVO_ON_ANGLE);
          vaultOpen = true;
          for (int i = 0; i < NUMPIXELS; i++) {
            strip.setPixelColor(i, strip.Color(255, 0, 0));  // Grün
          }
          strip.show();
        } else if (strikes == 0 && balls == 0) {
          lcd.print("Out");
          for (int i = 0; i < NUMPIXELS; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 255));  // Blau
          }
          strip.show();
          delay(2000);
        } else {
          lcd.print("S:" + String(strikes) + " B:" + String(balls));
          for (int i = 0; i < NUMPIXELS; i++) {
            strip.setPixelColor(i, strip.Color(0, 255, 0));  // Rot
          }
          strip.show();
          delay(2000);
        }
      }
      userInput = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Vault Code");
    }

    else if (key == 'B') {
      userInput = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input cleared");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Vault Code");
    }

    else if (key == 'D') {
      myServo.write(SERVO_OFF_ANGLE);
      digitalWrite(greenLedPin, LOW);
      vaultOpen = false;
      generateSecretCode();
      for (int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0);
      }
      strip.show();
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

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Code: " + secretCode);

      myServo.write(SERVO_ON_ANGLE);
      vaultOpen = true;

      for (int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); 
      }
      strip.show();

      delay(3000); 

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vault is");
      lcd.setCursor(0, 1);
      lcd.print("now open");
    }


  }
}

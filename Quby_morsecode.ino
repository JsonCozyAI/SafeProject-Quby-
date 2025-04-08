#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// -------------------- Tastenfeld Setup --------------------
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

// -------------------- LCD Setup --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------------------- Servo Setup --------------------
Servo myServo;
const int servoPin = 10;
const int SERVO_ON_ANGLE = 90;
const int SERVO_OFF_ANGLE = 0;

// -------------------- NeoPixel Setup --------------------
#define NUMPIXELS 80
#define LED_PIN 11
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_RGBW + NEO_KHZ800);

// -------------------- Variablen --------------------
String secretCode = "";
String userInput = "";
bool vaultOpen = false;

// Morsecode-Mapping (nur Zahlen 0-9)
String morseMap[10] = {
  "-----",  //0
  ".----",  //1
  "..---",  //2
  "...--",  //3 
  "....-",  //4
  ".....",  //5
  "-....",  //6
  "--...",  //7
  "---..",  //8
  "----."   //9
};

// -------------------- Funktion: Morsecode anzeigen --------------------
void showMorseCode(String code) {
  // LCD zeigt Morsecode-Hinweis
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morsecode being");
  lcd.setCursor(0, 1);
  lcd.print("displayed...");

  delay(1000);

  for (int i = 0; i < code.length(); i++) {
    char digit = code[i];
    String morse = morseMap[digit - '0'];

    for (int j = 0; j < morse.length(); j++) {
      if (morse[j] == '.') {
        strip.fill(strip.Color(255, 255, 255)); // Weißer Punkt
        strip.show();
        delay(200);
      } else if (morse[j] == '-') {
        strip.fill(strip.Color(255, 255, 255)); // Weißer Strich
        strip.show();
        delay(600);
      }
      strip.clear();
      strip.show();
      delay(200); // Pause zwischen Zeichen
    }
    delay(800); // Pause zwischen Zahlen
  }

  // Nach der Anzeige zur Eingabe auffordern
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter the code:");
}

// -------------------- Funktion: Geheimcode generieren --------------------
void generateSecretCode() {
  secretCode = "";
  int digits[10] = {0,1,2,3,4,5,6,7,8,9};
  for (int i = 9; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = digits[i];
    digits[i] = digits[j];
    digits[j] = temp;
  }
  for (int i = 0; i < 3; i++) {
    secretCode += String(digits[i]);
  }
}

// -------------------- Setup --------------------
void setup() {
  randomSeed(analogRead(0));
  lcd.init();
  lcd.backlight();

  myServo.attach(servoPin);
  myServo.write(SERVO_OFF_ANGLE);

  strip.begin();
  strip.setBrightness(30);
  strip.clear();
  strip.show();

  Serial.begin(9600);

  generateSecretCode();       // Zufälliger 3-stelliger Code
  showMorseCode(secretCode);  // Beim Start anzeigen
}

// -------------------- Loop --------------------
void loop() {
  char key = keypad.getKey();
  if (key) {
    // Zifferntaste gedrückt
    if (key >= '0' && key <= '9') {
      if (userInput.length() < 3) {
        userInput += key;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Input: " + userInput);
      }
    }

    // Taste A: Code überprüfen
    else if (key == 'A') {
      lcd.clear();
      lcd.setCursor(0, 0);
      if (userInput == secretCode) {
        lcd.print("Correct!");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vault is");
        lcd.setCursor(0, 1);
        lcd.print("now open");

        vaultOpen = true;
        myServo.write(SERVO_ON_ANGLE);
        strip.fill(strip.Color(255, 0, 0)); // Grün bei Erfolg
        strip.show();
      } else {
        lcd.print("Wrong code!");
        strip.fill(strip.Color(0, 255, 0)); // Rot bei Fehler
        strip.show();
        delay(2000);
        strip.clear();
        strip.show();
      }
      userInput = "";
    }

    // Taste #: Morsecode erneut anzeigen
    else if (key == '#') {
      showMorseCode(secretCode);
    }

    // Taste *: Lösung anzeigen und Tresor öffnen
    else if (key == '*') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Code: " + secretCode);
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vault is");
      lcd.setCursor(0, 1);
      lcd.print("now open");

      vaultOpen = true;
      myServo.write(SERVO_ON_ANGLE);
      strip.fill(strip.Color(255, 0, 0)); // Grün
      strip.show();
    }

    // Taste D: Tresor schließen & Code zurücksetzen
    else if (key == 'D') {
      myServo.write(SERVO_OFF_ANGLE);
      vaultOpen = false;
      generateSecretCode();          // Neuer Code
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vault closed!");
      delay(1500);
      showMorseCode(secretCode);     // Morsecode neu anzeigen
    }
  }
}

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 8
#define OUTPUT_PIN 7
#define RX_PIN 2
#define TX_PIN 3
#define CORRECT_PASSWORD "1234"

Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN); // SoftwareSerial for Arduino Uno to ESP32 communication

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {4, 5, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200); // Initiate serial communication
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522

  pinMode(OUTPUT_PIN, OUTPUT); // Set pin for digital output
  myservo.attach(SERVO_PIN); // Attach servo to pin
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  arduinoSerial.begin(9600); // Initialize SoftwareSerial
  delay(1000); // Allow time for initialization
}

void loop() {
  // Check for incoming commands from ESP32
  if (arduinoSerial.available()) {
    String command = arduinoSerial.readStringUntil('\n');
    command.trim();

    if (command == "UNLOCK") {
      unlockDoor("Remote Unlock");
    } else if (command == "CHECK_CONNECTION") {
      arduinoSerial.println("CONNECTED SUCCESSFULLY");
    }
  }

  // Check for password entry from keypad
  checkPasswordEntry();

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show UID on serial monitor and LCD
  String content = getCardUID();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID tag :");
  lcd.setCursor(0, 1);
  lcd.print(content);

  // Send UID to ESP32
  Serial.print("UID: ");
  Serial.println(content);

  // Check if the card is authorized
  if (content == "A3DBEB0D" || content == "892C1816" || content == "057BD703") {
    unlockDoor(content);
  } else {
    denyAccess();
  }
}

void unlockDoor(String message) {
  Serial.println("Authorized access");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  lcd.setCursor(0, 1);
  lcd.print(message);

  digitalWrite(OUTPUT_PIN, HIGH); // Use pin for digital output
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    delay(5);
  }
  delay(1000);
  for (int pos = 180; pos >= 0; pos--) {
    myservo.write(pos);
    delay(5);
  }
  digitalWrite(OUTPUT_PIN, LOW); // Use pin for digital output
}

void denyAccess() {
  Serial.println("Access denied");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  delay(1000);
}

String getCardUID() {
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    content += String(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();
  return content;
}

void checkPasswordEntry() {
  static String enteredPassword = "";
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      if (enteredPassword == CORRECT_PASSWORD) {
        unlockDoor("Correct Password");
      } else {
        denyAccess();
      }
      enteredPassword = ""; // Reset the entered password
    } else if (key == '*') {
      enteredPassword = ""; // Clear the entered password
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password");
    } else {
      enteredPassword += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password: ");
      lcd.print(enteredPassword);
    }
  }
}

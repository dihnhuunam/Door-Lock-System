#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <EEPROM.h>

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

enum Mode { NORMAL, ADD_CARD, REMOVE_CARD };
Mode currentMode = NORMAL;

void checkESP32Commands() {
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
      // Switch between modes
      switch (currentMode) {
        case NORMAL:
          currentMode = ADD_CARD;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Add Card Mode");
          break;
        case ADD_CARD:
          currentMode = REMOVE_CARD;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Remove Card Mode");
          break;
        case REMOVE_CARD:
          currentMode = NORMAL;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Scan your card");
          break;
      }
    } else {
      enteredPassword += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password: ");
      lcd.print(enteredPassword);
    }
  }
}

void checkRFIDCard() {
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

  if (currentMode == ADD_CARD) {
    addCard(content);
  } else if (currentMode == REMOVE_CARD) {
    removeCard(content);
  } else {
    // Check if the card is authorized
    if (isCardAuthorized(content)) {
      unlockDoor(content);
    } else {
      denyAccess();
    }
  }
  currentMode = NORMAL;
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

bool isCardAuthorized(String cardUID) {
  for (int i = 0; i < EEPROM.length(); i += 8) {
    String storedCard = "";
    for (int j = 0; j < 8; j++) {
      char c = EEPROM.read(i + j);
      if (c == 0) break;
      storedCard += c;
    }
    if (storedCard == cardUID) {
      return true;
    }
  }
  return false;
}

void addCard(String cardUID) {
  if (isCardAuthorized(cardUID)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card already");
    lcd.setCursor(0, 1);
    lcd.print("authorized");
  } else {
    for (int i = 0; i < EEPROM.length(); i += 8) {
      if (EEPROM.read(i) == 0) {
        for (int j = 0; j < cardUID.length() && j < 8; j++) {
          EEPROM.write(i + j, cardUID[j]);
        }
        EEPROM.write(i + cardUID.length(), 0); // Null-terminate the string
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Card Added");
        Serial.print("Card added: ");
        Serial.println(cardUID);
        return;
      }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Memory Full");
  }
}

void removeCard(String cardUID) {
  if (!isCardAuthorized(cardUID)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card not found");
    Serial.print("Card not found: ");
    Serial.println(cardUID);
  } else {
    for (int i = 0; i < EEPROM.length(); i += 8) {
      String storedCard = "";
      for (int j = 0; j < 8; j++) {
        char c = EEPROM.read(i + j);
        if (c == 0) break;
        storedCard += c;
      }
      if (storedCard == cardUID) {
        for (int j = 0; j < 8; j++) {
          EEPROM.write(i + j, 0);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Card Removed");
        Serial.print("Card removed: ");
        Serial.println(cardUID);
        return;
      }
    }
  }
}

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

  // Debug: Clear EEPROM for testing purposes
  // Uncomment this to reset the EEPROM if necessary
  // for (int i = 0; i < EEPROM.length(); i++) {
  //   EEPROM.write(i, 0);
  // }
}

void loop() {
  checkESP32Commands();
  checkPasswordEntry();
  checkRFIDCard();
}

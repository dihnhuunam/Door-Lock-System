#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <EEPROM.h>

#define SS_PIN 10    // Slave Select for SPI, used to select the RFID module.
#define RST_PIN 9    // Reset pin for the RFID module.
#define SERVO_PIN 8  // Pin connected to the servo.
#define OUTPUT_PIN 0 // Pin for digital output signal.
#define RX_PIN 2     // RX pin for software serial communication.
#define TX_PIN 3     // TX pin for software serial communication.

Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {4, 5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum Mode
{
  NORMAL,
  ADD_CARD,
  REMOVE_CARD,
  CHANGE_PASSWORD,
  VERIFY_PASSWORD
};
Mode currentMode = NORMAL;

String correctPassword = ""; // This will be fetched from the server
bool passwordVerified = false; // Track if the password has been verified

void checkESP32Commands();
void checkPasswordEntry();
void checkRFIDCard();
void unlockDoor(String message);
void denyAccess();
String getCardUID();
bool isCardAuthorized(String cardUID);
void addCard(String cardUID);
void removeCard(String cardUID);
void verifyPassword();

void setup()
{
  SPI.begin();
  mfrc522.PCD_Init();
  myservo.attach(SERVO_PIN);
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  arduinoSerial.begin(9600); // Initialize SoftwareSerial communication with ESP32
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
  delay(1000);
  pinMode(OUTPUT_PIN, OUTPUT); // Set the OUTPUT_PIN as an output
  digitalWrite(OUTPUT_PIN, LOW); // Ensure the output is initially off

  // Check connection with ESP32
  arduinoSerial.println("CHECK_CONNECTION");
}

void loop()
{
  checkESP32Commands();
  checkPasswordEntry();
  checkRFIDCard();
}

// Function implementations

void checkESP32Commands()
{
  // Check for incoming commands from ESP32
  if (arduinoSerial.available())
  {
    String command = arduinoSerial.readStringUntil('\n');
    command.trim();

    if (command == "UNLOCK")
    {
      unlockDoor("Remote Unlock");
    }
    else if (command == "CONNECTED SUCCESSFULLY")
    {
      Serial.println("ESP32 Connected");
    }
    else if (command.startsWith("SET_PASSWORD:"))
    {
      correctPassword = command.substring(13); // Extract the password
      Serial.print("New Password Set: ");
      Serial.println(correctPassword);
    }
    else if (command == "INCORRECT_OLD_PASSWORD")
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect Old Pw");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Old Password:");
    }
    else if (command == "PASSWORD_CHANGE_FAILED")
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Change Failed");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Old Password:");
    }
  }
}

void checkPasswordEntry()
{
  static String enteredPassword = "";
  static String oldPassword = "";
  static String newPassword = "";
  static bool awaitingNewPassword = false;
  char key = keypad.getKey();

  if (key)
  {
    if (key == '#')
    {
      if (currentMode == CHANGE_PASSWORD && awaitingNewPassword)
      {
        newPassword = enteredPassword;
        enteredPassword = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Changing Pw...");
        // Send the change password request to ESP32
        arduinoSerial.println("CHANGE_PASSWORD:" + oldPassword + ":" + newPassword);
        delay(1000);
        currentMode = NORMAL;
        awaitingNewPassword = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Password:");
      }
      else if (currentMode == CHANGE_PASSWORD && !awaitingNewPassword)
      {
        if (enteredPassword == correctPassword)
        {
          oldPassword = enteredPassword;
          enteredPassword = "";
          awaitingNewPassword = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("New Password:");
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Old Password:");
          enteredPassword = "";
        }
      }
      else if (currentMode == VERIFY_PASSWORD)
      {
        if (enteredPassword == correctPassword)
        {
          passwordVerified = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Verified");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Mode: Add-Remove");
          currentMode = NORMAL; // Reset mode to normal after verification
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Password:");
        }
        enteredPassword = "";
      }
      else
      {
        // Send the entered password to ESP32
        arduinoSerial.println(enteredPassword);

        if (enteredPassword == correctPassword)
        {
          unlockDoor("Correct Password");
        }
        else
        {
          denyAccess();
        }
      }
      enteredPassword = ""; // Reset the entered password
    }
    else if (key == 'A')
    {
      if (passwordVerified)
      {
        // Switch between modes
        switch (currentMode)
        {
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
          lcd.print("Mode: Add/Remove");
          break;
        }
      }
      else
      {
        currentMode = VERIFY_PASSWORD;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Password:");
      }
    }
    else if (key == '*')
    {
      currentMode = CHANGE_PASSWORD;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Old Password:");
    }
    else if (key == 'B')
    {
      // Delete the last entered digit
      if (enteredPassword.length() > 0)
      {
        enteredPassword.remove(enteredPassword.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        if (currentMode == CHANGE_PASSWORD && awaitingNewPassword)
        {
          lcd.print("New Password:");
        }
        else if (currentMode == CHANGE_PASSWORD)
        {
          lcd.print("Old Password:");
        }
        else if (currentMode == VERIFY_PASSWORD)
        {
          lcd.print("Enter Password:");
        }
        else
        {
          lcd.print("Password:");
        }
        lcd.setCursor(0, 1); // Display password on second line
        lcd.print(enteredPassword);
      }
    }
    else if (key == 'C')
    {
      // Clear all entered digits
      enteredPassword = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      if (currentMode == CHANGE_PASSWORD && awaitingNewPassword)
      {
        lcd.print("New Password:");
      }
      else if (currentMode == CHANGE_PASSWORD)
      {
        lcd.print("Old Password:");
      }
      else if (currentMode == VERIFY_PASSWORD)
      {
        lcd.print("Enter Password:");
      }
      else
      {
        lcd.print("Password:");
      }
      lcd.setCursor(0, 1); // Display password on second line
    }
    else if (key == 'D')
    {
      // Return to password entry screen
      currentMode = NORMAL;
      enteredPassword = "";
      newPassword = "";
      awaitingNewPassword = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    }
    else
    {
      enteredPassword += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      if (currentMode == CHANGE_PASSWORD && awaitingNewPassword)
      {
        lcd.print("New Password:");
      }
      else if (currentMode == CHANGE_PASSWORD)
      {
        lcd.print("Old Password:");
      }
      else if (currentMode == VERIFY_PASSWORD)
      {
        lcd.print("Enter Password:");
      }
      else
      {
        lcd.print("Password:");
      }
      lcd.setCursor(0, 1); // Display password on second line
      lcd.print(enteredPassword);
    }
  }
}

void checkRFIDCard()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
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
  arduinoSerial.println("UID:" + content);

  if (currentMode == ADD_CARD)
  {
    addCard(content);
  }
  else if (currentMode == REMOVE_CARD)
  {
    removeCard(content);
  }
  else
  {
    // Check if the card is authorized
    if (isCardAuthorized(content))
    {
      unlockDoor(content);
    }
    else
    {
      denyAccess();
    }
  }
  currentMode = NORMAL;
}

void unlockDoor(String message)
{
  Serial.println("Authorized access");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  lcd.setCursor(0, 1);
  lcd.print(message);

  digitalWrite(OUTPUT_PIN, HIGH); // Use pin for digital output
  for (int pos = 0; pos <= 180; pos++)
  {
    myservo.write(pos);
    delay(5);
  }
  delay(1000);
  for (int pos = 180; pos >= 0; pos--)
  {
    myservo.write(pos);
    delay(5);
  }
  digitalWrite(OUTPUT_PIN, LOW); // Use pin for digital output
}

void denyAccess()
{
  Serial.println("Access denied");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  delay(1000);
}

String getCardUID()
{
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    content += String(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();
  return content;
}

bool isCardAuthorized(String cardUID)
{
  for (int i = 0; i < EEPROM.length(); i += 8)
  {
    String storedCard = "";
    for (int j = 0; j < 8; j++)
    {
      storedCard += char(EEPROM.read(i + j));
    }
    if (storedCard == cardUID)
    {
      return true;
    }
  }
  return false;
}

void addCard(String cardUID)
{
  for (int i = 0; i < EEPROM.length(); i += 8)
  {
    String storedCard = "";
    for (int j = 0; j < 8; j++)
    {
      storedCard += char(EEPROM.read(i + j));
    }
    if (storedCard == cardUID)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card Already Added");
      delay(1000);
      return;
    }
    if (storedCard == "")
    {
      for (int j = 0; j < 8; j++)
      {
        EEPROM.write(i + j, cardUID[j]);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card Added");
      delay(1000);
      return;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Memory Full");
  delay(1000);
}

void removeCard(String cardUID)
{
  for (int i = 0; i < EEPROM.length(); i += 8)
  {
    String storedCard = "";
    for (int j = 0; j < 8; j++)
    {
      storedCard += char(EEPROM.read(i + j));
    }
    if (storedCard == cardUID)
    {
      for (int j = 0; j < 8; j++)
      {
        EEPROM.write(i + j, 0);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card Removed");
      delay(1000);
      return;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Not Found");
  delay(1000);
}

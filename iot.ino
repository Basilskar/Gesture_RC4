// Libraries Used
#include <Wire.h>
#include "paj7620.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define GES_REACTION_TIME 500
#define GES_ENTRY_TIME 800
#define GES_QUIT_TIME 1000

// EEPROM addresses
#define EEPROM_ENCRYPTED_ADDR 0

int Contrast = 60;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
char code[4] = {'9', '9', '9', '9'};
int keycount = 0;
int wrongcount = 0;

// RC4 encryption key
const byte encryptionKey[] = {0xA3, 0xB7, 0xC9, 0xD1, 0xE5, 0xF8, 0x12, 0x34};
const byte keyLength = sizeof(encryptionKey);

// Default password "3278" stored as encrypted bytes
byte encryptedPassword[4];

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smile[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

// RC4 state
byte rc4State[256];

// Function declarations
void rc4Initialize(const byte* key, byte keyLength);
void rc4Crypt(const byte* input, byte* output, byte length);
void encryptPassword(const char* password, byte* output);
bool checkEnteredPassword();
void saveEncryptedPassword();
void loadEncryptedPassword();
void printByteArrayAsHex(const byte* array, int length);

void setup() {
  analogWrite(6, Contrast);
  lcd.begin(16, 2);

  uint8_t error = 0;

  Serial.begin(9600);
  Serial.println("\nPAJ7620U2 TEST DEMO: Recognize 9 gestures with RC4 encryption.");

  error = paj7620Init(); // initialize Paj7620 registers
  if (error) {
    Serial.print("INIT ERROR, CODE:");
    Serial.println(error);
  } else {
    Serial.println("INIT OK");
  }

  // Initialize the RC4 encryption algorithm
  rc4Initialize(encryptionKey, keyLength);
  
  // Encrypt the default password "3278"
  encryptPassword("3278", encryptedPassword);

  
  lcd.setCursor(0, 0);
  lcd.print("Enter gesture");

  Serial.println("Please input your gestures:\n");

  lcd.createChar(1, heart);
  lcd.createChar(2, smile);
  lcd.setCursor(0, 1);
  lcd.write(1);
  
  Serial.println("System ready with RC4 encryption.");
}

void loop() {
  uint8_t data = 0, data1 = 0, error;
  error = paj7620ReadReg(0x43, 1, &data);
  if (!error) {
    switch (data) {
      case GES_RIGHT_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Forward");
          code[keycount] = '0';
          keycount++;
          delay(GES_QUIT_TIME);
        } else if (data == GES_BACKWARD_FLAG) {
          lcd.clear();
          code[keycount] = '1';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Backward");
          delay(GES_QUIT_TIME);
        } else {
          lcd.clear();
          code[keycount] = '8';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Right");
          Serial.println("Right");
          delay(GES_QUIT_TIME);
        }
        break;

      case GES_LEFT_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG) {
          lcd.clear();
          code[keycount] = '0';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Forward");
          delay(GES_QUIT_TIME);
        } else if (data == GES_BACKWARD_FLAG) {
          lcd.clear();
          code[keycount] = '1';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Backward");
          delay(GES_QUIT_TIME);
        } else {
          code[keycount] = '7';
          keycount++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Left");
          Serial.println("Left");
          delay(GES_QUIT_TIME);
        }
        break;

      case GES_UP_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG) {
          lcd.clear();
          code[keycount] = '0';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Forward");
          delay(GES_QUIT_TIME);
        } else if (data == GES_BACKWARD_FLAG) {
          lcd.clear();
          code[keycount] = '1';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Backward");
          delay(GES_QUIT_TIME);
        } else {
          code[keycount] = '5';
          keycount++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Up");
        }
        break;

      case GES_DOWN_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG) {
          lcd.clear();
          code[keycount] = '0';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Forward");
          delay(GES_QUIT_TIME);
        } else if (data == GES_BACKWARD_FLAG) {
          lcd.clear();
          code[keycount] = '1';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Backward");
          delay(GES_QUIT_TIME);
        } else {
          code[keycount] = '6';
          keycount++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Down");
        }
        break;

      case GES_FORWARD_FLAG:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Forward");
        code[keycount] = '0';
        keycount++;
        delay(GES_QUIT_TIME);
        break;

      case GES_BACKWARD_FLAG:
        lcd.clear();
        code[keycount] = '1';
        keycount++;
        lcd.setCursor(0, 0);
        lcd.print("Backward");
        delay(GES_QUIT_TIME);
        break;

      case GES_CLOCKWISE_FLAG:
        lcd.clear();
        code[keycount] = '2';
        keycount++;
        lcd.setCursor(0, 0);
        lcd.print("Clockwise");
        Serial.println("Clockwise");
        break;

      case GES_COUNT_CLOCKWISE_FLAG:
        lcd.clear();
        code[keycount] = '3';
        keycount++;
        lcd.setCursor(0, 0);
        lcd.print("Anti-clockwise");
        Serial.println("Anti-clockwise");
        break;

      default:
        paj7620ReadReg(0x44, 1, &data1);
        if (data1 == GES_WAVE_FLAG) {
          lcd.clear();
          code[keycount] = '4';
          keycount++;
          lcd.setCursor(0, 0);
          lcd.print("Wave");
          Serial.println("Wave");
        }
        break;
    }
  }

  // Check if 4 gestures have been entered
  if (keycount == 4) {
    checkEnteredPassword();
  }

  delay(100);
}

// Helper function to print byte arrays as hexadecimal
void printByteArrayAsHex(const byte* array, int length) {
  for (int i = 0; i < length; i++) {
    if (array[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(array[i], HEX);
    if (i < length - 1) {
      Serial.print(" ");
    }
  }
}

// Initialize RC4 with the encryption key
void rc4Initialize(const byte* key, byte keyLength) {
  byte j = 0;
  
  // Initialize state array
  for (int i = 0; i < 256; i++) {
    rc4State[i] = i;
  }
  
  // Shuffle the state array using the key
  for (int i = 0; i < 256; i++) {
    j = (j + rc4State[i] + key[i % keyLength]) % 256;
    // Swap rc4State[i] and rc4State[j]
    byte temp = rc4State[i];
    rc4State[i] = rc4State[j];
    rc4State[j] = temp;
  }
}

// RC4 encryption/decryption (symmetric algorithm)
void rc4Crypt(const byte* input, byte* output, byte length) {
  byte i = 0;
  byte j = 0;
  byte temp;
  
  // Create a temporary state array for this encryption operation
  byte tempState[256];
  memcpy(tempState, rc4State, 256);
  
  for (byte k = 0; k < length; k++) {
    i = (i + 1) % 256;
    j = (j + tempState[i]) % 256;
    
    // Swap tempState[i] and tempState[j]
    temp = tempState[i];
    tempState[i] = tempState[j];
    tempState[j] = temp;
    
    // XOR with generated stream byte
    byte stream = tempState[(tempState[i] + tempState[j]) % 256];
    output[k] = input[k] ^ stream;
  }
}

// Encrypt the password
void encryptPassword(const char* password, byte* output) {
  byte input[4];
  for (int i = 0; i < 4; i++) {
    input[i] = password[i];
  }
  
  rc4Crypt(input, output, 4);
}

// Check if entered password matches the stored encrypted password
bool checkEnteredPassword() {
  byte enteredPasswordBytes[4];
  byte decryptedPassword[4];
  
  // Convert entered code to bytes
  for (int i = 0; i < 4; i++) {
    enteredPasswordBytes[i] = code[i];
  }
  
  // Print the entered code in plain text
  Serial.print("Entered code: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(code[i]);
  }
  Serial.println();
  
  // Encrypt the entered password (RC4 is symmetric, so we use same function)
  byte encryptedEnteredPassword[4];
  rc4Crypt(enteredPasswordBytes, encryptedEnteredPassword, 4);
  
  // Print the encrypted entered password
  Serial.print("RC4 Ciphertext: ");
  printByteArrayAsHex(encryptedEnteredPassword, 4);
  Serial.println();
  
  // Print the expected encrypted password for comparison
  Serial.print("Expected Ciphertext: ");
  printByteArrayAsHex(encryptedPassword, 4);
  Serial.println();
  
  // Compare encrypted entered password with stored encrypted password
  bool passwordCorrect = true;
  for (int i = 0; i < 4; i++) {
    if (encryptedEnteredPassword[i] != encryptedPassword[i]) {
      passwordCorrect = false;
      break;
    }
  }

  if (passwordCorrect && wrongcount < 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unlocked");
    Serial.println("Status: Unlocked");
    keycount = 0;
    for (int i = 0; i < 4; i++) {
      code[i] = '9';
    }
    lcd.write(2);
    return true;
  } else {
    if (wrongcount > 1) {
      lcd.setCursor(0, 0);
      lcd.print("Completely locked");
      Serial.println("Status: Completely locked");
      keycount = 0;
      for (int i = 0; i < 4; i++) {
        code[i] = '9';
      }
    } else if (wrongcount > 0) {
      lcd.setCursor(0, 0);
      lcd.print("Security alert");
      Serial.println("Status: Security alert");
      wrongcount++;
      keycount = 0;
      for (int i = 0; i < 4; i++) {
        code[i] = '9';
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Wrong, locked");
      Serial.println("Status: Wrong, locked");
      wrongcount++;
      keycount = 0;
      for (int i = 0; i < 4; i++) {
        code[i] = '9';
      }
    }
    return false;
  }
}

// Save encrypted password to EEPROM
void saveEncryptedPassword() {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_ENCRYPTED_ADDR + i, encryptedPassword[i]);
  }
}

// Load encrypted password from EEPROM
void loadEncryptedPassword() {
  for (int i = 0; i < 4; i++) {
    encryptedPassword[i] = EEPROM.read(EEPROM_ENCRYPTED_ADDR + i);
  }
}

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 0  // Reset pin (connected to ESP32 GPIO pin D0)
#define SS_PIN 5   // Slave Select pin (connected to ESP32 GPIO pin D5)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Hard-coded list of IDs
String allowedIDs[3] = { "9922762026", "9822762026", "9722762026" };
String sId;
byte blockAddr = 2;
byte sector = 1;
MFRC522::StatusCode status;
enum Mode {
  STANDBY,
  AUTHENTICATION,
  RESET,
  REGISTRATION
};

Mode currentMode = STANDBY;  // Initial mode is standby

unsigned long modeStartTime = 0;  // Variable to keep track of mode start time
MFRC522::MIFARE_Key key;
void setup() {
  Serial.begin(9600);  // Initialize serial communication
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  Serial.println("Device ready. Enter 'mfr-r' to enter standby mode.");
}

void loop() {
  switch (currentMode) {
    case STANDBY:
      standbyMode();
      break;
    case AUTHENTICATION:
      authenticationMode();
      break;
    case RESET:
      resetMode();
      break;
    case REGISTRATION:
      registrationMode();
      break;
  }
}

void standbyMode() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.equals("mfr-r")) {
      currentMode = STANDBY;
      Serial.println("Already in Standby Mode.");
    } else if (command.equals("mfr-auth")) {
      currentMode = AUTHENTICATION;
      Serial.println("Entered Authentication Mode.");
    } else if (command.equals("mfr-R")) {
      currentMode = RESET;
      modeStartTime = millis();
      Serial.println("Entered Reset Mode.");
    } else if (command.startsWith("mfr-sid")) {
      String studentID = command.substring(8);
      Serial.println(studentID);
      // Extract student ID from command
      if (studentID.length() == 10) {
        // Perform registration here by writing studentID to card
        sId = studentID;
        Serial.println(sId);
        currentMode = REGISTRATION;
        modeStartTime = millis();
        Serial.println("Entered Registration Mode.");
      } else {
        Serial.println("Invalid student ID format.");
      }
    }
  }

  // Check RFID in standby mode (Do nothing)
}
int extract4Bits(byte inputByte, bool extractRight) {
  int extractedValue = 0;
  byte mask = 0x0F;  // Mask to extract the rightmost 4 bits by default
  if (!extractRight) {
    mask = 0xF0;  // Mask to extract the leftmost 4 bits
  }
  extractedValue = inputByte & mask;  // Extract the desired bits
  if (!extractRight) {
    extractedValue >>= 4;  // Shift the bits to the rightmost side for leftmost bits extraction
  }
  return extractedValue;
}
void authenticationMode() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector, mfrc522.uid.uidByte, mfrc522.uid.size);
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
      if (status == MFRC522::STATUS_OK) {
        String numberString = "";
        for (int i = 0; i < 5; i++) {

          int leftInt = extract4Bits(buffer[i], 0);
          Serial.println(String(leftInt));
          numberString.concat(String(leftInt));
          int rightInt = extract4Bits(buffer[i], 1);
          Serial.println(String(rightInt));
          numberString.concat(String(rightInt));
        }
        // Convert the numeric value back to the original 10-digit string
        Serial.print("Read number from card: ");
        Serial.println(numberString);
        for (int i = 0; i < 3; i++) {
          if (allowedIDs[i].equals(numberString)) {
            Serial.println("id found");
            return;
          }
        }
        Serial.println("id not found");
        // Check if cardID exists in allowedIDs array
        // Print success or does not exist based on result
      } else {
        Serial.println("Reading failed during authentication.");
      }
    } else {
      Serial.println("Authentication failed.");
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  } else if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.equals("mfr-r")) {
      currentMode = STANDBY;
      Serial.println("gone to standby mode.");
    }
  }
}

void resetMode() {
  if (millis() - modeStartTime >= 10000) {
    currentMode = STANDBY;
    Serial.println("Reset mode timeout. Back to Standby Mode.");
  } else {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    };
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    } else Serial.println(F("PCD_Authenticate() success: "));
    byte dataToWrite[16] = { 0 };  // Initialize data array to 0
    unsigned long long numberValue = sId.toInt();
    status = mfrc522.MIFARE_Write(2, dataToWrite, 16);
    if (status == MFRC522::STATUS_OK) {
      Serial.println("Data successfully written to the card.");
    } else {
      Serial.println("Failed to write data to the card." + String(status));
      return;
    }
    // Perform registration action on card
  }
}
byte insert4Bits(byte inputByte, bool insertRight, int value) {
  // Serial.println(String(value).concat(String(insertRight)));
  byte mask = 0xF0;            // Mask to clear the rightmost 4 bits
  byte bits4 = (value & 0xF);  // Extract the rightmost 4 bits of the value

  if (!insertRight) {
    // Clear the leftmost 4 bits of the byte
    mask = 0x0F;  // Mask to clear the leftmost 4 bits
    bits4 <<= 4;  // Shift the bits to be inserted to the leftmost side
  }

  // Clear the corresponding 4 bits in inputByte
  inputByte &= mask;
  // Merge bits4 into inputByte
  inputByte |= bits4;

  return inputByte;
}

void registrationMode() {
  if (millis() - modeStartTime >= 10000) {
    currentMode = STANDBY;
    Serial.println("Registration mode timeout. Back to Standby Mode.");
  } else {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    } else Serial.println(F("PCD_Authenticate() success: "));
    byte dataToWrite[16] = { 0 };  // Initialize data array to 0
    unsigned long long numberValue = sId.toInt();
    for (int i=0 ; i < sId.length(); i++) {
      char tempnum = sId.charAt(i);
      Serial.println("bool insert right:");
      Serial.println((i % 2) == 0);
      //  Serial.println("i is:");
        Serial.println(i / 2 );
      dataToWrite[(i / 2)]= insert4Bits(dataToWrite[i / 2], (i % 2) != 0, String(tempnum).toInt());
      int leftInt = extract4Bits(dataToWrite[i / 2], (i % 2) != 0);
      Serial.println(String(leftInt));
    }

    status = mfrc522.MIFARE_Write(2, dataToWrite, 16);
    if (status == MFRC522::STATUS_OK) {
      Serial.println("Data successfully written to the card.");
    } else {
      Serial.println("Failed to write data to the card." + String(status));
      return;
    }
    // Perform registration action on card
  }
}

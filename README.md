# 🎓 RFID Student ID Authentication System (ESP32 + MFRC522)

This Arduino-based project uses an **ESP32** and **MFRC522 RFID reader** to authenticate and register student IDs to RFID cards. It supports multiple operational modes like standby, authentication, registration, and reset via serial commands. Each RFID card stores a compressed 10-digit student ID using 4-bit encoding per digit.

---

## 🛠 Features

- ✅ **Multiple Operating Modes**
  - `Standby`: Idle listening for commands
  - `Authentication`: Scan RFID cards and verify IDs
  - `Registration`: Write new student IDs to RFID cards
  - `Reset`: Clear ID data from RFID cards

- 🆔 **Hardcoded Access Control**
  - Matches scanned card IDs against a predefined whitelist

- 💾 **4-bit Data Packing**
  - Each student ID digit is compressed into 4 bits to store two digits per byte efficiently

- 🕹️ **Serial-Based Mode Switching**
  - Commands such as `mfr-auth`, `mfr-sid<id>`, `mfr-R`, etc., control system modes via the Serial Monitor

---

## ⚙️ Hardware

- ESP32 development board  
- MFRC522 RFID module  
- MIFARE Classic 1K RFID cards  
- Jumper wires and breadboard

---

## 🔌 Pin Configuration

| MFRC522 Pin | ESP32 GPIO |
|-------------|------------|
| RST         | GPIO 0     |
| SDA (SS)    | GPIO 5     |
| MOSI        | GPIO 23    |
| MISO        | GPIO 19    |
| SCK         | GPIO 18    |
| GND         | GND        |
| 3.3V        | 3.3V       |

> Make sure to use **3.3V** only, as MFRC522 modules are not 5V-tolerant.

---

## 🧪 Serial Commands

- `mfr-auth`: Enter **Authentication Mode**
- `mfr-sid<10-digit-ID>`: Enter **Registration Mode** with student ID
- `mfr-R`: Enter **Reset Mode**
- `mfr-r`: Return to **Standby Mode**

---

## 📦 Code Structure

- `STANDBY`: Waits for user input
- `AUTHENTICATION`: Reads card, decodes student ID, checks against allowed list
- `REGISTRATION`: Encodes 10-digit ID into RFID card (2 digits per byte)
- `RESET`: Clears card data by writing zeroed bytes

---

## 📂 How it Works

1. **Student ID Storage**:
   - 10-digit IDs are compressed using 4-bit encoding.
   - Stored on block 2 of the RFID card.

2. **Authentication**:
   - Reads the same block, decompresses it back into the original ID.
   - Compares with `allowedIDs[]`.

3. **Security**:
   - Uses default key `0xFF 0xFF 0xFF 0xFF 0xFF 0xFF` for authentication.
   - Cards are halted after each operation for safe communication.

---

## 🧠 Example

```bash
> mfr-sid9922762026
# Enters registration mode
# Scan a new RFID card to write the ID

> mfr-auth
# Scan card
# Matches and prints "id found" or "id not found"

Project Overview

This project is an RFID-based authentication system using an ESP32 and MFRC522 RFID module. It allows users to:

Scan RFID cards for authentication

Register new RFID cards with student IDs

Reset or update RFID card data

Switch between different modes via Serial Monitor

Features

✅ RFID Card Authentication - Checks if the scanned card is in a list of allowed IDs.
✅ Card Registration - Stores a student ID onto an RFID card.
✅ Reset Mode - Clears or modifies the stored card data.
✅ Mode Selection via Serial Commands - Users can control functionality via a terminal.

Components & Wiring :

![image](https://github.com/user-attachments/assets/1d55c2bf-716b-488e-a7f8-92a3882c6277)

How It Works

1️⃣ Setup Phase

Initializes SPI interface for communication.

Initializes MFRC522 RFID reader.

Loads a hardcoded list of allowed IDs.

2️⃣ Operating Modes

Standby Mode - Listens for Serial commands.

Authentication Mode - Reads a card and checks if it's allowed.

Registration Mode - Stores a new 10-digit Student ID on an RFID card.

Reset Mode - Wipes or updates stored RFID data.

Serial Commands:

Command \t              Action

mfr-auth \t              Switch to Authentication Mode

mfr-sid 1234567890     Register Student ID 1234567890 to a card

mfr-R                  Switch to Reset Mode

Installation & Setup : 

Flash the code to an ESP32.

Connect the RFID module as per the wiring table.

Open the Serial Monitor (9600 baud rate).

Scan an RFID card or send Serial commands to interact.


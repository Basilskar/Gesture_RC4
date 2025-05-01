# Gesture-Based Door Lock System

## Overview
This project implements a gesture-controlled door lock system using an Arduino, a PAJ7620U2 gesture sensor, and an LCD display. When a valid gesture is detected, the system will lock or unlock the door and send email notifications for security events.

## Security Note

This project contains encrypted configuration information that can only be accessed with the correct password. If you're the owner of this repository, you'll need your personal password to decrypt the API key information.

## Features
* **Gesture Recognition:** Detects gestures using the PAJ7620U2 sensor
* **Lock Control:** Uses a servo motor to control the door lock mechanism
* **LCD Display:** Shows gesture name and lock status on a 16x2 LCD
* **Email Notifications:** Sends real-time email alerts on security events
* **Security Monitoring:** Detects wrong attempts, security alerts, and lockdowns

## Hardware Setup
Connect components according to this diagram:

```
Component      Arduino Pin
-----------------------
PAJ7620U2 VIN  5V
PAJ7620U2 GND  GND
PAJ7620U2 SDA  A4 (SDA)
PAJ7620U2 SCL  A5 (SCL)
LCD VCC        5V
LCD GND        GND
LCD SDA        A4 (SDA)
LCD SCL        A5 (SCL)
Servo Signal   D9
Potentiometer  A0
```

## Installation

1. **Clone the Repository:**
```bash
git clone https://github.com/yourusername/gesture-lock-system.git
cd gesture-lock-system
```

2. **Install Python Dependencies:**
```bash
pip install pyserial requests python-dotenv cryptography
```

3. **Configure Environment:**
   * Create a `.env` file with your API keys (see `.env.example`)
   * OR use the secure decryption tool (for repository owner only)

## Configuration (Repository Owner Only)

To access your encrypted configuration, run the decryption tool:

```bash
python decrypt_config.py
```

When prompted, enter your personal password to reveal your API keys.

<!-- 
ENCRYPTED_CONFIG:
U2FsdGVkX19KZGj8tMu3M2sTwAWMc2JFNBSV8l5Rpr5oONAn2X6XGsOHs7FDCpbL
ZpyaM3jq98x1QQxEWyeP1xzCFh8TgNf7mBMrB3QK/Mjk6yhK+LFgj72X3NU7XL2Q
CQSQ8N5TMPjEYcTmJnC+kvcW4lBbQA0oEjbRuE6YmYcLZ8M3sEP/bCQVdCKuCyUd
RJnhQGdlUEg9VpJ5ZHpuWqrqyB0e86MuVj5j0G1IMow=
-->

## Usage

1. **Upload Arduino Code:**
   * Upload `GestureLock.ino` to your Arduino

2. **Run the Monitor Script:**
```bash
python arduino_monitor.py
```

3. **Test System:**
   * The script will monitor for security events and send email notifications

## Troubleshooting

* **No Serial Connection:** Check if the correct COM port is specified
* **Emails Not Sending:** Verify Resend API key is correct and has necessary permissions
* **No Arduino Output:** Ensure your Arduino is sending the expected alert messages over Serial

## License
MIT License

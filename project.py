
import serial
import requests
import time
import json # Can be useful for debugging payload, though requests handles json directly

# --- !! HARDCODED CONFIGURATION (Demo Only) !! ---
# WARNING: Replace these placeholders with your actual values.
# WARNING: Do NOT share code with real keys hardcoded.

RESEND_API_KEY = "re_cTgfNpE5_P7ooiRpp59HzaprWn3oKPTVF"  # <--- REPLACE THIS with your actual Resend API Key
SENDER_EMAIL = "onboarding@resend.dev" # <--- REPLACE THIS (use your verified Resend sender: "Name <email>" or just "email")
RECIPIENT_EMAIL = "basilskar1960@gmail.com"   # <--- REPLACE THIS with the email address to receive alerts
SERIAL_PORT = "COM3"            # <--- REPLACE THIS with your Arduino's serial port (e.g., "COM3" on Windows)
BAUD_RATE = 9600                        # Should match Serial.begin() in Arduino code

# --- End of Hardcoded Configuration ---

# --- Input Validation (Basic Check) ---
if "YOUR_API_KEY_HERE" in RESEND_API_KEY or "onboarding@resend.dev" in SENDER_EMAIL or "delivered@resend.dev" in RECIPIENT_EMAIL:
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    print("!!! WARNING: Placeholder values detected in configuration. !!!")
    print("!!! Please replace them with your actual Resend details.   !!!")
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    # Consider exiting if placeholders are found:
    # import sys
    # sys.exit(1)

if not RESEND_API_KEY.startswith("re_"):
     print("Warning: RESEND_API_KEY doesn't seem to start with 're_'. Ensure it's correct.")


# --- Constants ---
RESEND_API_URL = "https://api.resend.com/emails"
# List of trigger phrases from Arduino's Serial output
ALERT_TRIGGERS = [
    "Status: Wrong, locked",
    "Status: Security alert",
    "Status: Completely locked"
]
NOTIFICATION_COOLDOWN_SECONDS = 300 # 5 minutes cooldown between notifications

# --- State ---
last_notification_time = 0

# --- Functions ---

def send_resend_email(subject, body_html):
    """Sends an email using the Resend API with hardcoded config."""
    global last_notification_time

    current_time = time.time()
    if current_time - last_notification_time < NOTIFICATION_COOLDOWN_SECONDS:
        print(f"Cooldown active. Notification '{subject}' suppressed.")
        return False

    # Prepare headers
    headers = {
        "Authorization": f"Bearer {RESEND_API_KEY}", # Using the hardcoded key
        "Content-Type": "application/json",
    }

    # Prepare the data payload
    payload = {
        "from": SENDER_EMAIL,       # Using the hardcoded sender
        "to": [RECIPIENT_EMAIL],    # Using the hardcoded recipient (needs to be a list)
        "subject": subject,
        "html": body_html,
    }

    try:
        print(f"Sending notification via Resend: Subject='{subject}'")
        # print(f" -> Payload: {json.dumps(payload, indent=2)}") # Uncomment for debugging payload

        # Make the POST request
        response = requests.post(RESEND_API_URL, headers=headers, json=payload)
        response.raise_for_status() # Raises HTTPError for bad responses (4xx or 5xx)

        print(f"Email sent successfully! Status Code: {response.status_code}")
        last_notification_time = current_time
        return True

    except requests.exceptions.HTTPError as http_err:
        print(f"HTTP Error occurred: {http_err}")
        try:
            error_details = response.json()
            print(f"Resend API Error Details: {error_details}")
        except ValueError:
             print(f"Resend API Response (non-JSON): {response.text}")
    except requests.exceptions.RequestException as req_err:
        print(f"An request error occurred: {req_err}")
    except Exception as e:
         print(f"An unexpected error occurred during email sending: {e}")

    return False

def listen_to_serial():
    """Listens to the serial port and triggers email on alert messages."""
    ser = None
    while True:
        try:
            print(f"Attempting to connect to serial port {SERIAL_PORT} at {BAUD_RATE} baud...")
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) # Using hardcoded port/baud
            print("Serial connection established. Waiting for data...")

            while True:
                if ser is None or not ser.is_open:
                    print("Serial port closed unexpectedly. Re-attempting connection...")
                    break

                try:
                    if ser.in_waiting > 0:
                        line_bytes = ser.readline()
                        if line_bytes:
                            line_str = line_bytes.decode('utf-8', errors='ignore').strip()
                            print(f"Received: {line_str}")

                            if line_str in ALERT_TRIGGERS:
                                print(f"Alert detected: {line_str}")
                                subject = f"Arduino Security Alert: {line_str.replace('Status: ', '')}"
                                body = f"""
                                <h1>Security Alert Detected</h1>
                                <p>Event on Arduino at {SERIAL_PORT}:</p>
                                <p><strong>{line_str}</strong></p>
                                <p>Timestamp: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
                                """
                                send_resend_email(subject, body)
                    else:
                        time.sleep(0.1) # Short pause

                except serial.SerialException as e:
                    print(f"Serial error during read: {e}. Attempting to reconnect...")
                    if ser and ser.is_open: ser.close()
                    ser = None
                    time.sleep(5)
                    break
                except UnicodeDecodeError as e:
                    print(f"Serial data decode error: {e}. Skipping line: {line_bytes}")
                except Exception as e:
                    print(f"An unexpected error occurred while reading serial: {e}")
                    time.sleep(1)

        except serial.SerialException as e:
            print(f"Error opening serial port {SERIAL_PORT}: {e}")
            print("Check port name, connection, and permissions. Retrying in 10 seconds...")
            if ser and ser.is_open: ser.close()
            ser = None
            time.sleep(10)
        except KeyboardInterrupt:
            print("\nExiting script...")
            if ser and ser.is_open:
                ser.close()
                print("Serial port closed.")
            break # Exit the main loop
        except Exception as e:
            print(f"An critical error occurred in the main loop: {e}")
            print("Retrying connection in 10 seconds...")
            if ser and ser.is_open: ser.close()
            ser = None
            time.sleep(10)

# --- Main Execution ---
if __name__ == "__main__":
    print("--- Arduino Security Monitor (Hardcoded Demo) ---")
    print(f"WARNING: Using hardcoded configuration - DO NOT SHARE WITH REAL KEYS!")
    print(f"Monitoring Serial Port: {SERIAL_PORT}")
    print(f"Alert Recipient: {RECIPIENT_EMAIL}")
    print(f"Alert Sender: {SENDER_EMAIL}")
    print("-------------------------------------------------------------")
    listen_to_serial()

#include <LiquidCrystal.h> 
#include <Keypad.h> 
#include <SoftwareSerial.h> 
 
// Define LCD pins 
LiquidCrystal lcd(12, 11, 5, 9, 4, 2); 
 
// Define GSM pins 
SoftwareSerial SIM900(7, 8); // RX, TX 
 
// Define keypad settings 
const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = { 
  {'1', '2', '3'}, 
  {'4', '5', '6'}, 
  {'7', '8', '9'}, 
  {'*', '0', '#'} 
}; 
byte rowPins[ROWS] = {A3, A4, 10, 13}; 
byte colPins[COLS] = {A0, A1, A2}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); 
 
// Define motor pin 
#define MOTOR_PIN 3 
 
// System variables 
const String password = "1234"; // Set the correct password 
int attempts = 0; 
const int maxAttempts = 3; 
bool motorLocked = false; 
 
void setup() { 
  // Initialize LCD 
  lcd.begin(16, 2); 
  lcd.print("Enter Passcode:"); 
 
  // Initialize GSM 
 Serial.begin(9600);    // Communication with PC 
  SIM900.begin(9600);    // Communication with GSM module 
   
  Serial.println("Initializing GSM module..."); 
  delay(1000);           // Give time for GSM to initialize 
   
  // Check if the module responds 


 
  SIM900.println("AT"); 
  delay(1000); 
  if (SIM900.available()) { 
    Serial.println("GSM module connected!"); 
  } else { 
    Serial.println("GSM module not responding."); 
    while (1); // Stop if GSM is not connected 
  } 
 
  // Initialize motor 
  pinMode(MOTOR_PIN, OUTPUT); 
  digitalWrite(MOTOR_PIN, LOW); // Motor OFF by default 
 
  Serial.println("System Ready"); 
} 
 
void loop() { 
  if (motorLocked) { 
    return; // Motor is permanently locked 
  } 
 
  char key = keypad.getKey(); 
 
  if (key) { 
    static String enteredPassword = ""; 
 
    if (key == '#') { 
      if (enteredPassword == password) { 
        lcd.clear(); 
        lcd.print(" Access Granted "); 
        digitalWrite(MOTOR_PIN, HIGH); // Motor ON 
        attempts = 0; // Reset attempts 
        delay(1000); 
        lcd.clear(); 
        lcd.print("Enter Passcode:"); 
      } else { 
        attempts++; 
        lcd.clear(); 
        lcd.print("Wrong Passcode"); 
        digitalWrite(MOTOR_PIN, LOW); // Motor OFF 
        delay(1000); 
        lcd.clear(); 
        lcd.print("Enter Passcode:"); 
        if (attempts >= maxAttempts) { 
          lockMotor(); 
        } 
      } 

 
      enteredPassword = ""; // Clear entered passcode 
    } else if (key == '*') { 
      enteredPassword = ""; // Clear input 
      lcd.clear(); 
      lcd.print("Enter Passcode:"); 
    } else { 
      enteredPassword += key; 
      lcd.setCursor(0, 1); 
      lcd.print(enteredPassword); 
    } 
  } 
} 
 
void lockMotor() { 
  motorLocked = true; 
  digitalWrite(MOTOR_PIN, LOW); // Turn motor OFF 
  lcd.clear(); 
  lcd.print("Motor Locked"); 
  sendSMS("ALERT! Vehicle Theft Detected. Motor Locked."); 
} 
 
void sendSMS(String message) { 
 Serial.println("Sending SMS..."); 
  SIM900.println("AT+CMGS=\"+919353290688\""); // Replace with the recipient's phone number 
   // Wait for the prompt (>) 
  delay(1000); 
  SIM900.print(message); // Message content 
  delay(1000); 
   
  SIM900.write(26); // Send Ctrl+Z to complete the SMS 
  delay(1000);      // Wait for SMS to be sent 
   
  Serial.println("SMS Sent!"); 
  while (1); // Stop after sending the SMS 
}
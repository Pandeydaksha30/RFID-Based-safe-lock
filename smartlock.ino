#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- PIN SETUP ----------------
#define SS_PIN 10
#define RST_PIN 9
const int servoPin  = 3;
const int buzzerPin = 5;

// ---------------- COMPONENT OBJECTS ----------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo doorServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- VARIABLES ----------------
int accessCount = 0;

// ⭐ YOUR CARD UID HERE ⭐
String authorizedUID = "C65D0F05";

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  doorServo.attach(servoPin);
  doorServo.write(0);

  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card...");
  lcd.setCursor(0, 1);
  lcd.print("RFID Smart Door");

  Serial.println("System Ready — Scan card");
}

// ---------------- MAIN LOOP ----------------
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Read UID
  String readUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) readUID += "0";
    readUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  readUID.toUpperCase();

  Serial.print("Card UID: ");
  Serial.println(readUID);

  // Compare with your authorized UID
  if (readUID == authorizedUID) {
    unlockDoor();
  } else {
    wrongIDBuzz();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// ---------------- UNLOCK DOOR ----------------
void unlockDoor() {
  accessCount++;
  Serial.println("Access Granted");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");

  for (int pos = 0; pos <= 90; pos++) {
    doorServo.write(pos);
    delay(15);
  }

  lcd.setCursor(0, 1);
  lcd.print("Welcome!");
  delay(3000);

  lockDoor();
}

// ---------------- LOCK DOOR ----------------
void lockDoor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locking...");

  for (int pos = 90; pos >= 0; pos--) {
    doorServo.write(pos);
    delay(15);
  }

  tone(buzzerPin, 800, 100);
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card...");
  lcd.setCursor(0, 1);
  lcd.print("RFID Smart Door");

  Serial.println("Door Locked");
}

// ---------------- ACCESS DENIED ----------------
void wrongIDBuzz() {
  Serial.println("Access Denied!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");

  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 500, 200);
    delay(250);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card...");
  lcd.setCursor(0, 1);
  lcd.print("RFID Smart Door");
}

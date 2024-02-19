#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
int buzzerPin = 3;
int lederrorPin = 2;
// int ledPin = 4;
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

int code[] = {147, 237, 03, 23}; // This is the stored UID (Unlock Card)
int codeRead = 0;
String uidString;

void setup()
{
  Serial.begin(9600);
  SPI.begin();        // Init SPI bus
  rfid.PCD_Init();     // Init MFRC522
  Serial.println(F("Stamford University Robotics Club"));

  pinMode(buzzerPin, OUTPUT);
  pinMode(lederrorPin, OUTPUT);
 // pinMode(ledPin, OUTPUT);
}

void loop()
{
  if (rfid.PICC_IsNewCardPresent())
  {
    readRFID();
  }
  delay(100);
}

void readRFID()
{
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    delay(200);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(lederrorPin, HIGH);
    delay(3500);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(lederrorPin, LOW);
    return;
  }

  Serial.println("Scanned PICC's UID:");
  printDec(rfid.uid.uidByte, rfid.uid.size);
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);

  int i = 0;
  boolean match = true;

  while (i < rfid.uid.size)
  {
    Serial.println(int(rfid.uid.uidByte[i]));
    Serial.println(int(code[i]));

    if (!(int(rfid.uid.uidByte[i]) == int(code[i])))
    {
      match = false;
    }

    i++;
  }

  digitalWrite(buzzerPin, HIGH);  // Turn on the buzzer
  
  Serial.println("Buzzing");
  delay(200);                     // Make a sound for 200 milliseconds
  digitalWrite(buzzerPin, LOW);   // Turn off the buzzer
 

  if (match)
  {
    Serial.println("\n*** Unlocked ***");
    Serial.println("\nWelcome Al-Amin");
    delay(200);
    digitalWrite(lederrorPin, HIGH);
    delay(2000);
    digitalWrite(lederrorPin, LOW);
  }
  else
  {
    Serial.println("\nUnknown Card");
    Serial.println("Buzzing as unknown card, red LED");
int j = 0;
  while (j < 6)
  {
    delay(200);                    
    digitalWrite(buzzerPin, HIGH);  
    digitalWrite(lederrorPin, HIGH);
    delay(500);                     
    digitalWrite(buzzerPin, LOW);   
    digitalWrite(lederrorPin, LOW);
    j++;
  }
    
  }

  Serial.println("============================");

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

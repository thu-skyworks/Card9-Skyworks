#include "rfid.h"
#include <Wire.h>
#include <PN532_HSU.h>
#include "PN532.h"


static PN532_HSU pn532_hsu(Serial1);
static PN532 pn532(pn532_hsu);

uint8_t RFID::skeletonKey[8] = {0x64,0x1C,0x6D,0xDB};
uint8_t RFID::skeletonKeyLength = 4;


void RFID::Poll()
{
  boolean success;

  if(Found()){ //A card had been found previously
    return;
  }
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousPollMillis > 300){
    previousPollMillis = currentMillis;
  }else{
    return;
  }

  if(card == Card_14443B){
    if(pn532.stuCardIsPresent()){ //Card doesn't leave yet
      return;
    }else{
      pn532.resetConfigFor14443B();
      card = Card_None;
      SetLedOn(false);
    }
  }

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 200);
  if(card == Card_14443A){
    if(success){
      //Card doesn't leave yet since last detected
      pn532.inRelease(0); //Release all cards
      return;
    }else{
      card = Card_None;
      SetLedOn(false);
    }
  }

  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
    }
    Serial.println("");
    pn532.inRelease(0); //Release all cards
    card = Card_14443A;
    found = true;
    SetLedOn(true);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
  }

  static uint8_t AFI[] = {0};
  success = pn532.inListPassiveTarget(PN532_106KBPS_ISO14443B, sizeof(AFI) , AFI, 200);
  if (success) {
    uint8_t cardId[3]; 
    uint8_t expire[3]; 
    char studentId[11];

    pn532.inRelease();
    success = pn532.readTsighuaStuCard(cardId, expire, studentId);
    if(success){
      Serial.println("Found student card!");
      Serial.print("Student Number: "); Serial.println(studentId);
      for(int i=0; i<5; i++){
        uid[i] = studentId[i+5];
      }
      for(int i=0; i<3; i++){
        uid[i+5] = cardId[i];
      }
      uidLength = 8;
      card = Card_14443B;
      found = true;
      SetLedOn(true);
    }
  }else{

  }
}

bool RFID::SkeletonKey()
{
  if(card == Card_14443A){
    if(uidLength != skeletonKeyLength)
      return false;
    for(int i=0; i<uidLength; i++){
      if(uid[i] != skeletonKey[i])
        return false;
    }
    return true;
  }
  return false;
}

void RFID::Init()
{
  pn532.begin();

  uint32_t versiondata = pn532.getFirmwareVersion();
  if (! versiondata) {
    Serial.println("Didn't find PN53x board");
    return;
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.print((versiondata>>24) & 0xFF, HEX); 
  Serial.print(" with firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  pn532.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  pn532.SAMConfig();
}

void RFID::SetLedOn(bool on){
  if(on)
    pn532.writeGPIOP7(0);
  else
    pn532.writeGPIOP7(1<<PN532_GPIO_P71);
}

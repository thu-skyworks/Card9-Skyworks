#include "rfid.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include "PN532.h"


static PN532_I2C pn532_i2c(Wire);
static PN532 pn532(pn532_i2c);

uint8_t RFID::skeletonKey[8] = {0x64,0x1C,0x6D,0xDB};
uint8_t RFID::skeletonKeyLength = 4;


void RFID::Poll()
{
  boolean success;

  if(uidLength > 0){ //A card had been found previously
    return;
  }
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousPollMillis > 300){
    previousPollMillis = currentMillis;
  }else{
    return;
  }

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 10);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
    }
    Serial.println("");
    // Wait 1 second before continuing
    // delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
    uidLength = 0;
  }
}

bool RFID::SkeletonKey()
{
  if(uidLength > 0){
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

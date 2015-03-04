#ifndef RFID_H__
#define RFID_H__

#include "Arduino.h"


class RFID
{
    enum{Card_None, Card_14443A, Card_14443B};
    static uint8_t skeletonKey[8];
    static uint8_t skeletonKeyLength;
    uint8_t uid[8];  // Buffer to store the returned UID
    uint8_t uidLength;                    // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    uint8_t card;
    bool found;
    unsigned long previousPollMillis;
public:
    RFID():uidLength(0),previousPollMillis(0),card(Card_None),found(false){}
    void Init();
    void Poll();
    bool Found()
    {
        return found;
    }
    uint8_t* GetUid(uint8_t &len)
    {
        len = uidLength;
        return uid;
    }
    bool SkeletonKey();
    void Next()
    {
        found = false;
    }
    void SetLedOn(bool on); 
};

#endif

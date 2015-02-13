#ifndef RFID_H__
#define RFID_H__

#include "Arduino.h"


class RFID
{
    static uint8_t skeletonKey[8];
    static uint8_t skeletonKeyLength;
    uint8_t uid[8];  // Buffer to store the returned UID
    uint8_t uidLength;                    // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    unsigned long previousPollMillis;
public:
    RFID():uidLength(0),previousPollMillis(0){}
    void Init();
    void Poll();
    bool Found()
    {
        return uidLength != 0;
    }
    uint8_t* GetUid(uint8_t &len)
    {
        len = uidLength;
        return uid;
    }
    bool SkeletonKey();
    void Next()
    {
        uidLength = 0;
    }
};

#endif

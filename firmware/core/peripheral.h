#ifndef PERIPHERAL_H__
#define PERIPHERAL_H__
#include "Arduino.h"

#define lockMagnetPin 12
#define lockDetectPin 4
#define buttonPin     5
#define alarmPin      6

#define DetectorSwitchDelay 80

class Door
{
    enum DoorStateDef
    {
        DoorLocked, DoorPreparedOpen, DoorOpened
    };
    DoorStateDef state;
    unsigned long lastDectTime;
    unsigned char lastDectVal;
    bool detect(); //return true if lock detected door opened
public:
    Door():state(DoorOpened){}
    void Init();
    void Open();
    bool UpdateState(); //return true if door is opened while locked
};

class Alarm
{
    bool on;
    void setAlarm(bool);
public:
    Alarm():on(false){}
    void Init();
    void On(){
        setAlarm(true);
    }
    void Off(){
        setAlarm(false);
    }
    bool IsAlarmOn(){
        return on;
    }
};

#endif

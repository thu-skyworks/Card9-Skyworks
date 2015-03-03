#ifndef PERIPHERAL_H__
#define PERIPHERAL_H__
#include "Arduino.h"

#define lockMagnetPin 12
#define lockDetectPin 4
#define buttonPin     5
#define alarmPin      6

#define DetectorSwitchDelay 80
#define LongPressThreshold 3000
#define DoorOpenTimeOut  5000

class Door
{
    enum DoorStateDef
    {
        DoorLocked, DoorPreparedOpen, DoorOpened
    };
    DoorStateDef state;
    unsigned long preparedTimer;
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

class Button
{
public:
    enum Action
    {
        ActionNone, ActionPressed, ActionLongPressed
    };
private:
    unsigned long lastChanged, pressTime;
    unsigned char lastState;
    Action latestAction;
public:
    void Init();
    Action LatestAction(){
        Action t = latestAction;
        latestAction = ActionNone;
        return t;
    }
    void Check();
};

#endif

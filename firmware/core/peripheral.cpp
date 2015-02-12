#include "peripheral.h"

void Door::Init()
{
    pinMode(lockMagnetPin, OUTPUT);
    pinMode(lockDetectPin, INPUT);
    digitalWrite(lockMagnetPin, HIGH);//lock door
    digitalWrite(lockDetectPin, HIGH);//enable pull-up

    lastDectVal = digitalRead(lockDetectPin);
    lastDectTime = millis();
}

void Door::Open()
{
    if (state == DoorLocked) {
        digitalWrite(lockMagnetPin, LOW);
        state = DoorPreparedOpen;
        Serial.println("DoorPreparedOpen");
    }
}

bool Door::detect()
{
    unsigned char ret = digitalRead(lockDetectPin);
    if(millis()-lastDectTime > DetectorSwitchDelay &&
        (ret ^ lastDectVal)){

        lastDectTime = millis();
        lastDectVal = ret;
    }
    return lastDectVal == HIGH;
}

bool Door::UpdateState()
{
    switch (state) {
    case DoorLocked:
        if (detect()) {
            state = DoorOpened;
            return true;
        }
        break;
    case DoorPreparedOpen:
        if (detect()) {
            digitalWrite(lockMagnetPin, HIGH);
            state = DoorOpened;
            Serial.println("DoorOpened");
        }
        break;
    case DoorOpened:
        if (!detect()) {
            digitalWrite(lockMagnetPin, HIGH);
            state = DoorLocked;
            Serial.println("DoorLocked");
        }
        break;
    }
    return false;
}

void Alarm::Init()
{
    pinMode(alarmPin, OUTPUT);
    digitalWrite(alarmPin, LOW);
}

void Alarm::setAlarm(bool _on)
{
    on = _on;
    digitalWrite(alarmPin, _on ? HIGH : LOW);
}

#include "peripheral.h"
#include "protocol.h"
#include "common.h"

void Door::Init()
{
    pinMode(lockMagnetPin, OUTPUT);
    pinMode(lockDetectPin, INPUT);
    digitalWrite(lockMagnetPin, HIGH);//lock door
    digitalWrite(lockDetectPin, HIGH);//enable pull-up

    delay(400);
    lastDectVal = digitalRead(lockDetectPin);
    lastDectTime = millis();
}

void Door::Open()
{
    if (state == DoorLocked) {
        digitalWrite(lockMagnetPin, LOW);
        preparedTimer = millis();
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
            Serial.println("DoorOpened Illegally");
            sendEventPacket(DoorDidOpen);
            return true;
        }
        break;
    case DoorPreparedOpen:
        if (millis() - preparedTimer > DoorOpenTimeOut) {
            digitalWrite(lockMagnetPin, HIGH);
            state = DoorOpened;
            Serial.println("DoorOpened");
            sendEventPacket(DoorDidOpen);
        }
        break;
    case DoorOpened:
        if (!detect()) {
            digitalWrite(lockMagnetPin, HIGH);
            state = DoorLocked;
            Serial.println("DoorLocked");
            sendEventPacket(DoorDidClose);
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
    if(on != _on){
        on = _on;
        digitalWrite(alarmPin, _on ? HIGH : LOW);
        sendEventPacket(_on ? AlarmDidOn : AlarmDidOff);
    }
}

void Button::Init()
{
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH);//enable pull-up
    lastState = digitalRead(buttonPin);
    lastChanged = millis();
    latestAction = ActionNone;
}

void Button::Check()
{
    unsigned long now = millis();
    uint8_t val = digitalRead(buttonPin);
    if((val^lastState) && now > lastChanged){
        lastChanged = now;
        lastState = val;
        if(val == LOW){ //key down
            pressTime = now;
            Serial.println("Button down");
        }else{ //key up
            if(now - pressTime > LongPressThreshold){
                latestAction = ActionLongPressed;
            }else{
                latestAction = ActionPressed;
            }
            Serial.println("Button up");
        }
    }
}



#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#define PacketIdentifier 0xA1
enum eventTypeDef
{
    CardDidScan,DoorDidOpen,DoorDidClose,DoorReleaseDidTriggered,AlarmDidOn,AlarmDidOff
};
enum commandTypeDef
{
    DoDoorOpen,DoAlarmOn,DoAlarmOff
};
enum requestTypeDef
{
    RequestAuth
};
enum responseTypeDef
{
    ResponseNegative,ResponsePositive,ResponseNatural
};
enum packetTypeDef
{
    PacketTypeEvent,PacketTypeCommand,PacketTypeRequest,PacketTypeResponse
};
struct eventPayload{
    uint8_t eventType; //0->cardDidScan 1->doorDidOpen 2->doorDidClose 3->doorReleaseDidTriggered 4->alarmDidOn 5->alarmDidOff 
};
struct commandPayload{
    uint8_t commandType; //0->doDoorOpen 1->doAlarmOn 2->doAlarmOff
};
struct requestPayload{
    uint8_t requestType; //0->auth
    unsigned char param[];
};
struct authParam{
    uint32_t cardId; //little_endian
};
struct responsePayload{
    uint8_t responseType; //0->negative 1->positive 2->natural
    unsigned char param[];
};
struct packet{
    uint8_t characteristicAndVersion; // must be 0xA1
    uint8_t type; //0->event 1->command 2->request 3->response
    uint16_t payloadSize; //little_endian
    union{
        struct eventPayload event;
        struct commandPayload command;
        struct requestPayload request;
        struct responsePayload response;
    }payload;
};
#endif


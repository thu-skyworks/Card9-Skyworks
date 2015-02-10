struct data{
	uint8_t characteristicAndVersion; // must be 0xA1
	uint8_t type; //0->event 1->command 2->request 3->response
	uint16_t extendDataSize; //little_endian
	unsigned char data[];
};
struct eventData{
	uint8_t eventType; //0->cardDidScan 1->doorDidOpen 2->doorDidClose 3->doorReleaseDidTriggered 4->alarmDidOn 5->alarmDidOff 
};
struct commandData{
	uint8_t commandType; //0->doDoorOpen 1->doAlarmOn 2->doAlarmOff
};
struct requestData{
	uint8_t requestType; //0->auth
	unsigned char param[];
};
struct authParam{
	uint32_t cardId; //little_endian
};
struct responseData{
	uint8_t responseType; //0->negative 1->positive 2->natural
	unsigned char param[];
};
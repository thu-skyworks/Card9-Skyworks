#include <SPI.h>
#include <Ethernet.h>

#include "protocol.h"
#include "common.h"
#include "peripheral.h"
#include "rfid.h"


const int led = 13; //LED pin config
byte mac[] = {0xDE, 0xAD, 0x00, 0x09, 0x00, 0x09};
#define SERVER_HOST "192.168.1.103"
#define SERVER_PORT 39999
#define MAX_CONNECT_RETRIES 5
#define MAX_PACKET_SIZE 16

EthernetClient client;
Door door;
Alarm alarm;
Button button;
RFID rfid;

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
}

void ErrorDhcp() //Indicates DHCP error occurred
{
  for(char i=0; i<10; i++){
      digitalWrite(led, HIGH);
      delay(200);
      digitalWrite(led, LOW);
      delay(200);
  }
}

void ErrorConnect() //Indicates connecting error occurred
{
  for(char i=0; i<4; i++){
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      delay(100);
  }
}

void setup() {
  pinMode(led, OUTPUT);
  door.Init();
  alarm.Init();
  button.Init();
 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // this check is only needed on the Leonardo:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Card9 controller started");

  rfid.Init();

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");

  while (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    ErrorDhcp();
    //software_Reset();
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  IPAddress ip = Ethernet.localIP();
  for (uint8_t thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(ip[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();

}

void parsePacket(packet* p)
{
  Serial.print(F("Packet type: "));
  Serial.println(p->type);
  switch(p->type){
    case PacketTypeCommand:
      switch (p->payload.command.commandType) {
          case DoDoorOpen:
            door.Open();
            break;
          case DoAlarmOn:
            alarm.On();
            break;
          case DoAlarmOff:
            alarm.Off();
            break;
      }
      break;
    case PacketTypeResponse:
      break;
  }
}

void sendEventPacket(uint8_t eventType)
{
  struct packet p;
  p.characteristicAndVersion = PacketIdentifier;
  p.type = PacketTypeEvent;
  p.payloadSize = sizeof(eventPayload);
  p.payload.event.eventType = eventType;
  client.write((byte*)&p, sizeof(struct packet));
}

void receivePacket(byte charRecved)
{
  static uint8_t packetBuffer[MAX_PACKET_SIZE];
  static uint8_t ptr;
  struct packet* pPacket = (struct packet*)packetBuffer;
  if(!ptr){
    if(charRecved == PacketIdentifier)
      packetBuffer[ptr++] = charRecved;
  }else if(ptr < MAX_PACKET_SIZE-1){
    packetBuffer[ptr++] = charRecved;
    if(ptr >= 4){ //payloadSize field already received
      if(pPacket->payloadSize == ptr - 4){ //packet receiving competed
        parsePacket(pPacket);
        ptr = 0; //receiving next packet
      }else if(pPacket->payloadSize > MAX_PACKET_SIZE - 4){
        Serial.println("Packet is too long");
        ptr = 0; //restart packet receiving
      }
    }
  }
}

void renewDhcp()
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > 30000){
    previousMillis = currentMillis; 
    Ethernet.maintain();
  }
}

void loop() {
  static uint8_t connect_retries = 0;

  button.Check();

  if(door.UpdateState()){
    alarm.On();
  }
  Button::Action a = button.LatestAction();
  if(a == Button::ActionPressed){
    sendEventPacket(DoorReleaseDidTriggered);
    door.Open();
  }else if(a == Button::ActionLongPressed){
    alarm.Off();
  }

  rfid.Poll();
  if(rfid.Found()){
    sendEventPacket(CardDidScan);
    if(rfid.SkeletonKey()){
      door.Open();
    }
    delay(500);
    rfid.Next();
  }

  if (!client.connected()) {
    digitalWrite(led, LOW);
    client.stop();
    
    Serial.println();
    Serial.println("Trying to connect server.");
    connect_retries++;
    if(client.connect(SERVER_HOST, SERVER_PORT)>0){
      Serial.println("connected");
      connect_retries = 0;
      digitalWrite(led, HIGH);
    }else{
      if(connect_retries <= MAX_CONNECT_RETRIES)
        return;
      ErrorConnect();
      //  software_Reset();
    }
  }
  while (client.available() > 0){
    receivePacket(client.read());
  }
  renewDhcp();
}


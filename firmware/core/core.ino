#include <SPI.h>
#include <Ethernet.h>

const int led = 13; //LED pin config
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#define SERVER_HOST "192.168.1.106"
#define SERVER_PORT 2828
#define MAX_CONNECT_RETRIES 5

EthernetClient client;

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
 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // this check is only needed on the Leonardo:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

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

void parseCmd(char charRecved)
{
}

void renewDhcp()
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > 30){
    previousMillis = currentMillis; 
    Ethernet.maintain();
  }
}

void loop() {
  static uint8_t connect_retries = 0;
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
    parseCmd(client.read());
  }
  renewDhcp();
}


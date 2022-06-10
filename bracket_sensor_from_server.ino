#include <SPI.h>
#include <Ethernet.h>
const int ETH_CS = 10;
const int SD_CS = 4;

float sensor1Voltage;
float sensor2Voltage;
int sensor1 = A0;
int sensor2 = A1;
char dataReq;
//MAC address printed on a sticker on the shield
byte mac[] = {0xA8, 0x61, 0X0A, 0xAE, 0x82, 0xF3}; 

IPAddress ip(10,12,75,190); //network IP address
IPAddress server(10,12,75,190); //set IP address of server
EthernetClient client; //establish arduino as a client

void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(ETH_CS,OUTPUT); 
  pinMode(SD_CS,OUTPUT); 
  digitalWrite(ETH_CS,LOW); // Select the Ethernet Module.
  digitalWrite(SD_CS,HIGH); // De-Select the internal SD Card
  // Start in DHCP Mode; "Dynamic Host Configuration Protocol" allows Home Network 
  //Security to host its own DHCP server to manage device IP addresses.
  if (Ethernet.begin(mac) == 0){  
    Serial.println("Failed to configure Ethernet using DHCP, using Static Mode");
    Ethernet.begin(mac, ip); // If DHCP Mode failed, start in Static Mode
  }
  delay(1000);
  Serial.println("connecting...");
  if (client.connect(server, 5000)){ //server port and client port should match
    Serial.println("connected");} 
  else{
    Serial.println("connection failed");}
}
void loop() {
  sensor1Voltage = (analogRead(sensor1) * 5.0) / 1023; //converts analog values to Voltage
  sensor2Voltage = (analogRead(sensor2) * 5.0) / 1023;
  Serial.print("Sensor1:");
  Serial.println(sensor1Voltage);
  delay(500);
  Serial.print("Sensor2: ");
  Serial.println(sensor2Voltage);
  delay(500);
  
  if(client.available()>0) //if MES has sent a request
  {
    dataReq = client.read(); //reads bytes from client
    client.print("SENSOR");
    client.write(dataReq); //echos back request
    client.print(": ");
    Serial.print("SENSOR");
    Serial.print(dataReq);
    Serial.print(": ");
    if (dataReq == '1') //requested sensor 1 
    {
      sensor1Voltage = (analogRead(sensor1) * 5.0) / 1023;
      if(sensor1Voltage < 2.15){ //when Voltage is low bracket is present
        client.println("PASS");
        Serial.println("PASS");
      }
      else{
        client.println("FAIL");  //tells server brackets not present
        Serial.println("FAIL");
      }
    }
    else if (dataReq == '2') //requested sensor 2
    {
      sensor2Voltage = (analogRead(sensor2) * 5.0) / 1023;
      if(sensor2Voltage < 2.15){
        client.println("PASS");
        Serial.println("PASS");
      }
      else{
        client.println("FAIL");
        Serial.println("FAIL");
      }
    }
    delay(500);
  }
}

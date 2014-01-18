#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 20);
IPAddress serverIp(255, 255, 255, 255);
unsigned int localPort = 8888;      // local port to listen on
char  ReplyBuffer[] = "acknowledged";       // a string to send back

EthernetUDP Udp;
EthernetServer server(4444);

void EhernetSetup() {
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);

  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void EhernetLoop(){
  readTcpClient();
  print_values();
}

void print_values()
{
  if (SmartWire.available() > 0) {
    SmartData data = SmartWire.readBuffer();
    
    Serial.print(SmartWire.errorCount);
    Serial.print("\t");
    Serial.print(data.length);
    Serial.print("\t");
    Serial.println(millis());
    for (int a=0; a<data.length; a++) {
      Serial.print(data.buffer[a], HEX);
      Serial.print(" ");
    }
    Serial.println();
    
    publishDataToUdp(data.buffer, data.length);
  }
}

void publishDataToUdp(unsigned char buffer[], int length) {
    Udp.beginPacket(serverIp, 12345);
    Udp.write("E");
    Udp.write(buffer, length);
    Udp.endPacket();
}

void readTcpClient(){
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    SmartWire.initEvent();
    while (client.connected()) {
      if (client.available()) {
        unsigned char c = client.read();
        SmartWire.writeToBuf(c);
        Serial.write(c);
      }
    }
    SmartWire.flush();
    client.stop();
    Serial.println("client disonnected");
  }
}


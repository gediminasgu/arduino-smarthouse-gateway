#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 20);
IPAddress serverIp(255, 255, 255, 255);
unsigned int localPort = 8888;      // local port to listen on
char  ReplyBuffer[] = "acknowledged";       // a string to send back

EthernetUDP Udp;

void WebServerSetup() {
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
}

void WebServerLoop(){
  /*
  String params;
  if (params = server.serviceRequest())
  {
    if (params.startsWith("?refresh"))
    {
      refreshValues();
    }
    else if (params.startsWith("?time"))
    {
      server.print("time");
    }
    else if (params.startsWith("?profile="))
    {
      setProfile(params.substring(params.indexOf("=")+1));
    }
    else {
      print_values();
    }
    server.respond();
  }*/
  print_values();
}
/*
void refreshValues() {
  justStarted = 1;
  server.print("Refreshing...");
}*/

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

/*  char charBuf[50];
  for (int a=0; a<measureCount; a++)
  {
    measures[a].toCharArray(charBuf, 50);
    server.print(charBuf);
    server.print("\n");
  }*/
}

void publishDataToUdp(unsigned char buffer[], int length) {
    Udp.beginPacket(serverIp, 12345);
    Udp.write("E");
    Udp.write(buffer, length);
    Udp.endPacket();
}
/*
void setProfile(String profile) {
  Serial.print("#profile:");
  Serial.println(profile);
  
  Wire.beginTransmission(0); // broadcast message
  Wire.write("#profile:");
  char buf[2];
  profile.toCharArray(buf, 2);
  Wire.write(buf);
  Wire.endTransmission();    // stop transmitting
  
  server.print("Setting profile...");
}*/
/*

uint16_t PrintTimeSet(uint8_t *buf)
{
        int i=0;
        uint16_t plen;
        plen=ess.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Trying to set time. Please call results query to get results.</h1>"));
        return(plen);
}
*/

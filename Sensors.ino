#include <stdlib.h>
#include <WSWire.h>
#include <SmartWire.h>
#include <RTClib.h>

#define OUTSITE_LIGHT_SENSOR 2
#define WIRE_ADDRESS 0x9

// Codes used when publishing events to SmartWire
const unsigned char CODE_GAS_SENSOR = 0x01;
const unsigned char CODE_OUTSITE_LIGHT_SENSOR = 0x02;

#define HOLDING_REGS_SIZE 20
unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array

int DEBUG = 0;
long lastMillis = 0;
int justStarted = 1;

int readBytes = 0;
unsigned char valueType;
RTC_DS1307 RTC;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Setting up");
  WebServerSetup();

  SmartWire.begin(WIRE_ADDRESS, HOLDING_REGS_SIZE, holdingRegs);
  
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  GasCounterSetup();

  Serial.println("Setup done");
}

long count = 0;
String line;
void loop(void)
{
  readInput();
  WebServerLoop();
  GasCounterLoop();

  if (justStarted==1 || (DEBUG==1) || lastMillis + 300000 < millis())
  {
    PrintDateTime();
    RefreshMeasures();
    lastMillis = millis();
    justStarted = 0;
  }
}

void readInput() {
  while (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if (line.length() > 0 || incomingByte == '#')
    {
      if (incomingByte == 13 || (incomingByte == '#' && line.length() > 0))
      {
        String command = line.substring(1);
        if (command=="debug") {
            DEBUG = 1;
        }
        else if (command=="nodebug") {
            DEBUG = 0;
        }
        else if (command=="refresh") {
            justStarted = 1;
        }
        line = "";
      }
      else
      {
        char c = incomingByte;
        line += c;
      }
    }
  }
}


void PrintDateTime() {
    DateTime now = RTC.now();
 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void RefreshMeasures()
{
  Serial.print("OUTSITE_LIGHT_SENSOR ");
  Serial.println(analogRead(OUTSITE_LIGHT_SENSOR));
  
    SmartWire.initEvent();
    SmartWire.writeToBuf((unsigned char)0x03);  // General float value
    SmartWire.writeToBuf(CODE_OUTSITE_LIGHT_SENSOR);
    float value = analogRead(OUTSITE_LIGHT_SENSOR) / 1.0;
    SmartWire.writeToBuf(value);
    SmartWire.flush();
    
    publishDataToUdp(SmartWire.frame, SmartWire.frameLength);

    //AddMeasure("MILLIS", millis());
    //AddMeasure("MEMORY", freeMem());
}

int memoryTest() {
  int byteCounter = 0; // initialize a counter
  byte *byteArray; // create a pointer to a byte array
  // More on pointers here: http://en.wikipedia.org/wiki/Pointer#C_pointers

  // use the malloc function to repeatedly attempt
  // allocating a certain number of bytes to memory
  // More on malloc here: http://en.wikipedia.org/wiki/Malloc
  while ( (byteArray = (byte*) malloc (byteCounter * sizeof(byte))) != NULL ) {
    byteCounter++; // if allocation was successful, then up the count for the next try
    free(byteArray); // free memory after allocating it
  }

  free(byteArray); // also free memory after the function finishes
  return byteCounter; // send back the highest number of bytes successfully allocated
}

extern char * const __brkval;

uint16_t freeMem(void)
{
  char *brkval;
  char *cp;

  brkval = __brkval;
  if (brkval == 0) {
    brkval = __malloc_heap_start;
  }
  cp = __malloc_heap_end;
  if (cp == 0) {
    cp = ((char *)AVR_STACK_POINTER_REG) - __malloc_margin;
  }
  if (cp <= brkval) return 0;

  return cp - brkval;
}


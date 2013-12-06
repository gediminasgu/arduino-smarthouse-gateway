#define GAS_SENSOR 1

int gasLastValue = 0;
int gasCount = 0;
long gasAnalogSum = 0;
int gasAnalogCount = 0;

void GasCounterSetup() {
}

void GasCounterLoop() {
  CheckGas();
}

void CheckGas()
{
  int gasSensor = analogRead(GAS_SENSOR);
  gasAnalogSum += gasSensor;
  gasAnalogCount++;

    if (DEBUG==1) {
      Serial.print("GAS: ");
      Serial.print(gasSensor);
      Serial.print("\t");
      Serial.print(gasCount);
      Serial.print("\t");
      Serial.println(gasLastValue);
    }
  
  if (gasAnalogCount >= 1000) {
    double avg = gasAnalogSum / gasAnalogCount;

    if (gasLastValue == 0 && avg < 300) {
      gasLastValue = 1;
      gasCount++;
      publishGasChange();
    }
    else if (gasLastValue == 1 && avg > 350) {
      gasLastValue = 0;
    }
    gasAnalogSum = 0;
    gasAnalogCount = 0;
  }
}

void publishGasChange() {
    Serial.print("GAS_SENSOR01 ");
    Serial.println(gasCount);
    Serial.print("GAS ");
    Serial.println(analogRead(GAS_SENSOR));
    
    SmartWire.initEvent();
    SmartWire.writeToBuf((unsigned char)0x03);  // General float value
    SmartWire.writeToBuf(CODE_GAS_SENSOR);
    float value = 0;
    if (gasCount > 0)
      value = gasCount / 100.0;
    SmartWire.writeToBuf(value);
    SmartWire.flush();
    
    publishDataToUdp(SmartWire.frame, SmartWire.frameLength);
}

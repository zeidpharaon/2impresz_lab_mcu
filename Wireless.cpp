#include "Wireless.h"

boolean waitNewData(){
  unsigned long now = millis();
  while(!radio.receiveDone() && millis() - now < TIMEOUT);
  if(millis() - now < TIMEOUT){
    return true;
  }else{
    Serial.println("time out");
    return false;
  }
}

void sendAcknoledge(){
  if (radio.ACKRequested())
   {
     radio.sendACK();
     //Serial.print(" - ACK sent\n");
   }
}
void floatToArray(byte * buff, float f, int start) {
  byte * p = (byte *)(&f);
  for (int i = 0; i < 4; i++) {
    buff[i + start] = *p;
    p++;
  }
}

void intToArray(byte * buff, int n, int start) {
  byte * p = (byte *)(&n);
  for (int i = 0; i < 2; i++) {
    buff[i + start] = *p;
    p++;
  }
}

void byteToArray(byte * buff, byte b, int start) {
  byte * p = (byte *)(&b);
  for (int i = 0; i < 1; i++) {
    buff[i + start] = *p;
    p++;
  }
}

float arrayToFloat(volatile uint8_t  * buff, int start) {
  float ret;
  uint8_t * p = (uint8_t *)(&ret);
  for (int i = 0; i < 4; i++) {
    *p = buff[i + start];
    p++;
  }
  return ret;
}

int arrayToInt(volatile uint8_t  * buff, int start) {
  int ret;
  uint8_t * p = (uint8_t *)(&ret);
  for (int i = 0; i < 2; i++) {
    *p = buff[i + start];
    p++;
  }
  return ret;
}

byte arrayToByte(volatile uint8_t  * buff, int start) {
  byte ret;
  uint8_t * p = (uint8_t *)(&ret);
  for (int i = 0; i < 1; i++) {
    *p = buff[i + start];
    p++;
  }
  return ret;
}

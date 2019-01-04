#ifndef Wireless_h
#define Wireless_h

#include <Arduino.h>
#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>           //included with Arduino IDE (www.arduino.cc)

#define NETWORKID 100
#define ROOF 1
#define LAB 2
#define FREQUENCY RF69_868MHZ
#define IDLE_MODE 0
#define RX_MODE 1
#define TX_MODE 2
#define TRANSMISSION_DONE 0xFF
#define SWEEPDATA 0xAA
#define TIMEDATA 0x55
#define TIMEOUT 1000

extern RFM69 radio;
boolean waitNewData();
void sendAcknoledge();


void floatToArray(byte * buff, float f, int start);
void intToArray(byte * buff, int n, int start);
void byteToArray(byte * buff, byte b, int start);
float arrayToFloat(volatile uint8_t  *, int = 0);
int arrayToInt(volatile uint8_t  * buff, int start);
byte arrayToByte(volatile uint8_t  * buff, int start);

#endif

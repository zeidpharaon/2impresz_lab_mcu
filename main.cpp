#include <Arduino.h>
#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>           //included with Arduino IDE (www.arduino.cc)
#include "Wireless.h"
#include "DataTypes.h"

#define button 3
#define NODEID LAB


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers
unsigned long tensec = 10000;
RFM69 radio;
boolean receiveData();
void printData(struct SweepData);
void send();
boolean connect();
void sendSweepUpdate(struct SweepInfo);



/*byte channel_no_update = 5;
float startVoltage = 0.1;
float endVoltage = 2;
float stepVoltage = 0.1;
int timeBetweenSweeps = 1;//unit ??*/



byte mode = IDLE_MODE;

/*float voltages[50];
float currents[50];*/

//boolean printData = false;

unsigned long start = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower();

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), send, FALLING);

  /*delay(10000);
  Serial.println("start");*/
}

void loop() {

  if (radio.receiveDone()){
      //Serial.println("received something");
      if(receiveData()){
         //Serial.println("SUCCES");
      }else{
        Serial.println("FAIL");
    }


 }

 if ( Serial.available() > 0){
     digitalWrite(LED_BUILTIN,HIGH);
     String temp_String = "";
     char termination = ' ';
      struct SweepInfo send;
     temp_String = Serial.readStringUntil(termination);
     if(temp_String.toInt() == 1){
       send.on = true;
     }else{
       send.on = false;
     }
     temp_String = Serial.readStringUntil(termination);
     uint16_t chan = temp_String.toInt();
     temp_String = Serial.readStringUntil(termination);
     send.endVoltage = temp_String.toFloat();
     temp_String = Serial.readStringUntil(termination);
     send.startVoltage = temp_String.toFloat();
     temp_String = Serial.readStringUntil(termination);
     send.stepVoltage = temp_String.toFloat() / 1000; //receiving milli volts divide by 100 to convert to volts
     temp_String = Serial.readStringUntil(termination);
     send.timeBetweenSweeps = temp_String.toInt();
     uint16_t mask = 0x0001;
     for(int i = 0; i < 16; i++){
       if((chan & mask) != 0){
         //Serial.println(i);
         send.channelNumber = i;
         if(connect()){
           sendSweepUpdate(send);
         }else{
           Serial.println("cant send sweep update :(");
         }
       }
       mask = mask << 1;
     }

     Serial.println("Received sweep update !!!");
     Serial.print("channel no : ");
     Serial.println(chan, BIN);
     Serial.print("time between sweep : ");
     Serial.println(send.timeBetweenSweeps);
   }

 //Serial.println("hi");
}

void send() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    Serial.println("Button Presssed !!");
    struct SweepInfo send;
    send.channelNumber = 11;
    send.startVoltage = 0;
    send.endVoltage = 2;
    send.stepVoltage = 0.1;
    send.timeBetweenSweeps = 1;
    send.on = true;
    if(connect()){
      sendSweepUpdate(send);
    }else{
      Serial.println("cant send sweep update :(");
    }

  }
}

void printData(struct SweepData data){
  Serial.print("Channel Number : ");
  Serial.println(data.channelNumber);
  Serial.print("Date : ");
  Serial.println(String(data.date.day) + "/" + String(data.date.month) + "/" + String(data.date.year) + "\t" + String(data.date.hour) + ":" + String(data.date.minute));
  Serial.print("Temperature : ");
  Serial.println(data.temperature);
  Serial.print("Hummidity : ");
  Serial.println(data.humidity);
  Serial.print("Intensity : ");
  Serial.println(data.intensity);
  Serial.print("Sweep Size : ");
  Serial.println(data.sweepSize);

  for(int i = 0; i < data.sweepSize; i++){
    Serial.print(data.voltages[i]);
    Serial.print("\t");
    Serial.println(data.currents[i]);
  }
}

/*boolean receiveData(struct SweepData *data){

  if(radio.DATALEN == 0){
    //continue
    sendAcknoledge();
  }else{
    return false;
  }

  if(!waitNewData()) return false;

 data->channelNumber = arrayToByte(radio.DATA, 3);
 data->date.year = arrayToByte(radio.DATA, 4);
 data->date.month = arrayToByte(radio.DATA, 5);
 data->date.day = arrayToByte(radio.DATA, 6);
 data->date.hour = arrayToByte(radio.DATA, 7);
 data->date.minute = arrayToByte(radio.DATA, 8);
 data->temperature = arrayToFloat(radio.DATA, 9);
 data->humidity = arrayToFloat(radio.DATA, 13);
 data->intensity = arrayToFloat(radio.DATA, 17);
 data->sweepSize = arrayToInt(radio.DATA, 21);

 sendAcknoledge();

 float v[data->sweepSize];
 float c[data->sweepSize];
 for(int p = 0; p < ceil(data->sweepSize / 7.0); p++){
   if(!waitNewData()) return false;
   for(int i = 0; i < 7; i++){
     v[p*7 + i] = arrayToFloat(radio.DATA, 3 + i*8);
     c[p*7 + i] = arrayToFloat(radio.DATA, 3 + i*8 + 4);

     if(p*7 + i+1 >= data->sweepSize){
       break;
     }
   }
   sendAcknoledge();
 }

 //data->voltages = v;
 //data->currents = c;
 //Serial.println("cvdvdv");

 for(int i = 0; i < data->sweepSize; i++){
   data->voltages[i] = v[i];
   data->currents[i] = c[i];
   //*(data->currents + i) = c[i];
 }
 //data->sweepSize = arrayToInt(radio.DATA, 21);
 return true;
}*/

boolean receiveData(){

  if(radio.DATALEN == 0){
    //continue
    sendAcknoledge();
  }else{
    return false;
  }

  if(!waitNewData()) return false;

  byte channelNumber = arrayToByte(radio.DATA, 3);
  byte year = arrayToByte(radio.DATA, 4);
  byte month = arrayToByte(radio.DATA, 5);
  byte day = arrayToByte(radio.DATA, 6);
  byte hour = arrayToByte(radio.DATA, 7);
  byte minute = arrayToByte(radio.DATA, 8);
  float temperature = arrayToFloat(radio.DATA, 9);
  float humidity = arrayToFloat(radio.DATA, 13);
  float intensity = arrayToFloat(radio.DATA, 17);
  int sweepSize = arrayToInt(radio.DATA, 21);

  Serial.print("Channel Number : ");
  Serial.println(channelNumber);
  Serial.print("Date : ");
  Serial.println(String(day) + "/" + String(month) + "/" + String(year) + "\t" + String(hour) + ":" + String(minute));
  Serial.print("Temperature : ");
  Serial.println(temperature);
  Serial.print("Hummidity : ");
  Serial.println(humidity);
  Serial.print("Intensity : ");
  Serial.println(intensity);
  Serial.print("Sweep Size : ");
  Serial.println(sweepSize);

  /*Serial.print(String(day) + "." + String(month) + "." + String(year) + "," + String(hour) + ":" + String(minute) + "," + String(channelNumber) + ",");
  Serial.print(String(temperature) + "," + String(humidity) + "," + String(intensity) + ",");*/

 sendAcknoledge();

 for(int p = 0; p < ceil(sweepSize / 7.0); p++){
   if(!waitNewData()) return false;
   for(int i = 0; i < 7; i++){
     Serial.print(arrayToFloat(radio.DATA, 3 + i*8));
     Serial.print('\t');
     Serial.println(arrayToFloat(radio.DATA, 3 + i*8 + 4), 10);//5?
     /*Serial.print(arrayToFloat(radio.DATA, 3 + i*8)) ;
     Serial.print(",");
     Serial.print(arrayToFloat(radio.DATA, 3 + i*8 + 4), 5);
     Serial.print(",");*/
     if(p*7 + i+1 >= sweepSize){
      //Serial.println("");
       break;
     }
   }
   sendAcknoledge();
 }


 return true;
}

void sendSweepUpdate(struct SweepInfo sen){

  byte data[61];
  intToArray(data, 0, 0);
  byteToArray(data, SWEEPDATA, 2);
  byteToArray(data, sen.channelNumber, 3);
  floatToArray(data, sen.startVoltage, 4);
  floatToArray(data, sen.endVoltage, 8);
  floatToArray(data, sen.stepVoltage, 12);
  intToArray(data, sen.timeBetweenSweeps, 16);
  int temp;
  if(sen.on == true){
    temp = 1;
  } else{
    temp = 0;
  }
  Serial.print("twmp = ");
  Serial.println(temp);
  byteToArray(data, temp, 18);


  if (radio.sendWithRetry(ROOF, data, 19, 5, 40)) {
    Serial.println("sent update succesfully");
  } else {
    Serial.println("send fail");
    return;
  }
}

boolean connect(){
  if (radio.sendWithRetry(ROOF, 0, 0, 10, 40)) {
    Serial.println("connection succeded");
    return true;
  } else {
    Serial.println("connection failed");
    return false;
  }
}

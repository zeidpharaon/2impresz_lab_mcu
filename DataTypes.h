#ifndef DataTypes_h
#define DataTypes_h

struct Date{
  int second;
  int minute;
  int hour;
  int day;
  int month;
  int year;
};

struct SweepData{
  byte channelNumber;
  struct Date date;
  float temperature;
  float humidity;
  float intensity;
  int sweepSize;
  float* voltages;
  float* currents;
};

struct SweepInfo{
  byte channelNumber;
  boolean on = false;
  float startVoltage;
  float endVoltage;
  float stepVoltage;
  int timeBetweenSweeps;//unit ??
};

struct ChannelInfo{
  struct SweepInfo sweepInfo;
  struct Date timeOfLastSweep;
};
#endif

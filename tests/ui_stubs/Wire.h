#pragma once
#include <stddef.h>
struct MockWire {
  bool pressed=false;
  int x=0,y=0,at=0;
  void begin(int,int,int){}
  void setTimeOut(int){}
  void beginTransmission(int){}
  void write(const uint8_t*,size_t){}
  int endTransmission(){return 0;}
  int requestFrom(int,uint8_t){at=0;return 8;}
  int read(){uint8_t p[]={0,uint8_t(pressed),uint8_t(x>>8),uint8_t(x),uint8_t(y>>8),uint8_t(y),0,0};return p[at++%8];}
};
inline MockWire Wire;

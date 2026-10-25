#pragma once
#include <stdint.h>
#include <algorithm>
#define GFX_NOT_DEFINED -1
inline uint16_t capturedFrame[480*320];
inline uint16_t physicalFrame[320*480];
inline int frameFlushes=0;
struct Arduino_ESP32QSPI {Arduino_ESP32QSPI(int,int,int,int,int,int){}};
struct Arduino_AXS15231B {Arduino_AXS15231B(Arduino_ESP32QSPI*,int,int,bool,int,int){}};
struct Arduino_Canvas {
 Arduino_Canvas(int,int,Arduino_AXS15231B*){}
 bool begin(int){return true;}
 void setRotation(int){}
 void fillScreen(uint16_t c){std::fill(capturedFrame,capturedFrame+480*320,c);}
 uint16_t* getFramebuffer(){return physicalFrame;}
 void flush(){frameFlushes++;for(int y=0;y<320;y++)for(int x=0;x<480;x++)capturedFrame[y*480+x]=physicalFrame[x*320+319-y];}
 void draw16bitRGBBitmap(int x,int y,uint16_t* p,int w,int h){for(int row=0;row<h;row++)std::copy(p+row*w,p+(row+1)*w,capturedFrame+(y+row)*480+x);}
};

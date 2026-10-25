#pragma once
#include <stdint.h>

// LVGL landscape -> native 320x480 portrait. Write contiguous PSRAM columns
// instead of striding 640 bytes between every destination pixel.
inline void copyLandscapeToPortrait(uint16_t* frame,const uint16_t* pixels,
                                   int x,int y,int width,int height,bool flipped=false) {
  for(int col=0;col<width;col++) {
    if(flipped) {
      auto destination=frame+(479-x-col)*320+y;
      for(int row=0;row<height;row++)destination[row]=pixels[row*width+col];
      continue;
    }
    auto destination=frame+(x+col)*320+(319-y);
    for(int row=0;row<height;row++)destination[-row]=pixels[row*width+col];
  }
}

#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include "Core.h"

bool decodeConfig(JsonVariantConst,bike::Config&,String& error);
void encodeConfig(JsonObject,const bike::Config&);

class Storage {
  SPIClass spi_{HSPI};
  uint32_t sequence_=0,lastSave_=0;
  bool readSlot(const char*,String&,uint32_t&);
public:
  bool mounted=false,healthy=false;
  String error;
  void begin(bike::Config&,bike::State&);
  bool save(const bike::Config&,const bike::State&,bool force=false);
};


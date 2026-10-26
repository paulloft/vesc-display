#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "Core.h"
class BikeNetwork {
  WebServer server_{80};
  String password_;
  bool uploadAllowed_=false,uploadOk_=false;
  bool auth();
public:
  using GetStatus=String(*)();
  using Apply=bool(*)(const String&,String&);
  using Ready=bool(*)();
  using Prepare=void(*)();
  String ssid;
  bool updating=false,online=false;
  const String& password()const{return password_;}
  void begin(GetStatus,Apply,Ready,Ready,Prepare,bool storageAvailable=false);
  void tick(){server_.handleClient();}
};


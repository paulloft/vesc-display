#pragma once
#include "Arduino.h"
#include "SPI.h"
#include <map>
#include <memory>
#include <algorithm>
#define FILE_READ "r"
class File {
  std::shared_ptr<std::string> data_;
  size_t at_=0;
public:
  File()=default;
  explicit File(std::shared_ptr<std::string> p):data_(p){}
  explicit operator bool()const{return bool(data_);}
  size_t size()const{return data_?data_->size():0;}
  int read(){return data_&&at_<data_->size()?uint8_t((*data_)[at_++]):-1;}
  size_t readBytes(char* p,size_t n){size_t got=0;while(got<n){int c=read();if(c<0)break;p[got++]=c;}return got;}
  size_t write(uint8_t c){if(!data_)return 0;data_->push_back(c);return 1;}
  size_t write(const uint8_t* p,size_t n){if(!data_)return 0;data_->append(reinterpret_cast<const char*>(p),n);return n;}
  void flush(){}
  void close(){data_.reset();}
};
struct MockSD {
  bool present=true;
  std::map<std::string,std::shared_ptr<std::string>> files;
  bool begin(int,SPIClass&,int){return present;}
  File open(const char* path,const char* mode){
    if(!present)return File();
    if(mode[0]=='w'){files[path]=std::make_shared<std::string>();return File(files[path]);}
    auto it=files.find(path);return it==files.end()?File():File(it->second);
  }
};
inline MockSD SD;

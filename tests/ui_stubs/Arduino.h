#pragma once
#include <stdint.h>
#ifdef __cplusplus
#include <string>
#include <cstring>
#include <type_traits>
#include <vector>
#include <deque>
class String : public std::string {
  using const_iterator=std::string::const_iterator;
public:
  using std::string::string;
  String(const char* p):std::string(p?p:""){}
  String(const std::string& v):std::string(v){}
  template<class T,typename std::enable_if<std::is_integral<T>::value,int>::type=0>
  String(T value):std::string(std::to_string(value)){}
  int indexOf(const String& text)const{auto i=find(text);return i==npos?-1:int(i);}
  bool concat(const char* p,size_t n){append(p,n);return true;}
  bool concat(const char* p){append(p);return true;}
};
class HardwareSerial {
public:
  static inline HardwareSerial* instance=nullptr;
  std::deque<uint8_t> incoming;
  std::vector<uint8_t> outgoing;
  explicit HardwareSerial(int){instance=this;}
  void setRxBufferSize(int){}
  void begin(uint32_t,int,int,int){}
  void end(){}
  int available(){return incoming.size();}
  int read(){int b=incoming.front();incoming.pop_front();return b;}
  void write(const uint8_t* p,size_t n){outgoing.insert(outgoing.end(),p,p+n);}
};
#define SERIAL_8N1 0
#define PI 3.14159265358979323846
extern "C" {
#endif
uint32_t millis(void);
#ifdef __cplusplus
}
inline bool psramFound(){return true;}
inline void pinMode(int,int){}
inline void digitalWrite(int,int){}
#define OUTPUT 1
#define LOW 0
#define HIGH 1
#endif

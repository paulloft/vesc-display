#include "Devices.h"
#include "Board.h"
#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
namespace {
esp_adc_cal_characteristics_t keyCalibration;
struct AdcSample {uint16_t raw;uint16_t noiseMv;};
AdcSample readKeyAdc() {
  // Give the sample capacitor multiple conversions to settle on the weak source.
  (void)adc1_get_raw(ADC1_CHANNEL_5);
  uint16_t values[9];
  for(auto& value:values){delayMicroseconds(5);value=adc1_get_raw(ADC1_CHANNEL_5);}
  uint16_t raw=bike::trimmedAdcMean(values);
  return {raw,uint16_t(esp_adc_cal_raw_to_voltage(values[8],&keyCalibration)-
                                     esp_adc_cal_raw_to_voltage(values[0],&keyCalibration))};
}
}

bool Loads::write(uint8_t reg,uint8_t value) {
  bus_.beginTransmission(0x20);bus_.write(reg);bus_.write(value);
  const bool ok=bus_.endTransmission()==0;if(!ok)errors++;return ok;
}
void Loads::begin(){bus_.begin(board::ioSda,board::ioScl,100000);bus_.setTimeOut(10);}
void Loads::off(){if(initialized_) {online=write(0x0a,0);sent_=0;}}
void Loads::tick(bike::State& state,const bike::Config& cfg,uint32_t now) {
  if(!initialized_) {
    if(now-retry_<1000)return;retry_=now;
    initialized_=write(0x0a,0)&&write(0x01,0)&&write(0x02,0)&&write(0x06,0)&&write(0,0x40);
    online=initialized_;sent_=0;
    if(!initialized_)return;
  }
  uint8_t desired=state.outputs(now,cfg);
  static constexpr int8_t bits[7]={0,1,2,5,4,-1,3};
  if(desired!=sent_) {
    uint8_t previous=sent_;
    if(!write(0x0a,desired)){online=false;initialized_=false;return;}
    sent_=desired;
    for(int i=0;i<7;i++)if(bits[i]>=0 && ((desired^previous)&desired&(1<<bits[i])))onAt_[i]=now;
  }
  if(now-poll_<5)return;poll_=now;
  bus_.beginTransmission(0x20);bus_.write(0x09);
  if(bus_.endTransmission(false)==0 && bus_.requestFrom(uint8_t(0x20),uint8_t(1))==1) {
    bool b=bus_.read()&0x40;if(b&&!brake)onAt_[5]=now;brake=b;online=true;
  } else {errors++;online=false;initialized_=false;return;}
  constexpr uint8_t commands[8]={0x84,0xc4,0x94,0xd4,0xa4,0xe4,0xb4,0xf4};
  const int ch=channel_;channel_=(channel_+1)%8;
  bus_.beginTransmission(0x48);bus_.write(commands[ch]);
  if(bus_.endTransmission(false)!=0 || bus_.requestFrom(uint8_t(0x48),uint8_t(1))!=1) {
    errors++;adcOnline=false;return;
  }
  raw[ch]=bus_.read();rawAt_[ch]=now;adcOnline=true;
  if(ch>=7)return;
  bool on=bits[ch]>=0 ? (sent_&(1<<bits[ch])) : brake;
  if(!on || now-onAt_[ch]<cfg.faultGraceMs || raw[ch]<cfg.faultThreshold) {
    faultTiming_[ch]=false;return;
  }
  if(!faultTiming_[ch]){faultTiming_[ch]=true;faultAt_[ch]=now;}
  else if(now-faultAt_[ch]>=cfg.faultConfirmMs)state.faults|=1<<ch;
}

void Keys::begin() {
  pinMode(board::keyPower,INPUT_PULLUP);pinMode(board::keyMode,INPUT_PULLUP);
  pinMode(board::keyPullup,INPUT_PULLUP);
  // Configure once: Arduino analogReadMilliVolts reconfigures and characterizes
  // the ADC on every sample. GPIO6 is ADC1 channel 5, independent of Wi-Fi.
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_6);
  esp_adc_cal_characterize(ADC_UNIT_1,ADC_ATTEN_DB_6,ADC_WIDTH_BIT_12,1100,&keyCalibration);
  // Passive continuity hint: with all keys released, IO6 should follow the
  // IO9 pull-down/up change. Never drive the remote net as an output.
  gpio_set_pull_mode(gpio_num_t(board::keyPullup),GPIO_PULLDOWN_ONLY);delay(8);
  pullupDownMv=esp_adc_cal_raw_to_voltage(readKeyAdc().raw,&keyCalibration);
  gpio_set_pull_mode(gpio_num_t(board::keyPullup),GPIO_PULLUP_ONLY);delay(8);
  pullupUpMv=esp_adc_cal_raw_to_voltage(readKeyAdc().raw,&keyCalibration);
  events_=xQueueCreate(64,sizeof(bike::Event));
  if(events_&&xTaskCreatePinnedToCore(worker,"remote-keys",4096,this,1,nullptr,0)!=pdPASS){vQueueDelete(events_);events_=nullptr;}
}
void Keys::enqueue(bike::Event event,void* context) {
  auto& keys=*static_cast<Keys*>(context);
  xQueueSend(keys.events_,&event,0);
}
void Keys::worker(void* context) {
  auto& keys=*static_cast<Keys*>(context);
  TickType_t wake=xTaskGetTickCount();
  for(;;) {
    portENTER_CRITICAL(&keys.configLock_);bike::Config config=keys.sampledConfig_;portEXIT_CRITICAL(&keys.configLock_);
    keys.sample(millis(),config,enqueue,&keys);
    vTaskDelayUntil(&wake,pdMS_TO_TICKS(5));
  }
}
void Keys::tick(uint32_t now,const bike::Config& c,bike::EventSink sink,void* ctx) {
  if(!events_) {sample(now,c,sink,ctx);return;}
  portENTER_CRITICAL(&configLock_);sampledConfig_=c;portEXIT_CRITICAL(&configLock_);
  bike::Event event;
  while(xQueueReceive(events_,&event,0)==pdTRUE)sink(event,ctx);
}
void Keys::sample(uint32_t now,const bike::Config& c,bike::EventSink sink,void* ctx) {
  if(now-last_<5)return;last_=now;
  pullupDigital=digitalRead(board::keyPullup);
  auto adc=readKeyAdc();
  ladder_.sample(esp_adc_cal_raw_to_voltage(adc.raw,&keyCalibration),adc.raw>=4080,
                 adc.noiseMv,now,c,sink,ctx);
  millivolts=ladder_.filteredMv;rawMillivolts=ladder_.rawMv;noiseMillivolts=ladder_.noiseMv;
  resistance=ladder_.resistance;decoded=ladder_.decoded;
  powerButton_.tick(!digitalRead(board::keyPower),now,bike::Key::Power,c,sink,ctx);
  modeButton_.tick(!digitalRead(board::keyMode),now,bike::Key::M,c,sink,ctx);
}

#include "Devices.h"
#include "Board.h"
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <driver/gpio.h>

namespace {
adc_oneshot_unit_handle_t keyAdc=nullptr;
adc_cali_handle_t keyCalibration=nullptr;
uint16_t keyVoltage(int raw) {
  int mv=0;
  if(keyCalibration&&adc_cali_raw_to_voltage(keyCalibration,raw,&mv)==ESP_OK)return uint16_t(mv);
  // ESP32-S3 nominal 6 dB range; per-device eFuse calibration is preferred.
  return uint16_t(uint32_t(raw)*1750/4095);
}
int keyRaw() {
  int raw=4095; // Failed reads release the ladder instead of inventing a press.
  if(keyAdc)adc_oneshot_read(keyAdc,ADC_CHANNEL_5,&raw);
  return raw;
}
struct AdcSample {uint16_t raw;uint16_t noiseMv;};
AdcSample readKeyAdc() {
  // Give the sample capacitor multiple conversions to settle on the weak source.
  (void)keyRaw();
  uint16_t values[9];
  for(auto& value:values){delayMicroseconds(5);value=keyRaw();}
  uint16_t raw=bike::trimmedAdcMean(values);
  return {raw,uint16_t(keyVoltage(values[8])-
                                     keyVoltage(values[0]))};
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
    if(now-retry_<1000)return;
    retry_=now;
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
  if(now-poll_<5)return;
  poll_=now;
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
  // ADC1 channel 5 is GPIO6 and stays available while Wi-Fi is active.
  // Create/calibrate once; the core 0 worker only performs conversions.
  adc_oneshot_unit_init_cfg_t unit{};unit.unit_id=ADC_UNIT_1;
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit,&keyAdc));
  adc_oneshot_chan_cfg_t channel{};channel.atten=ADC_ATTEN_DB_6;channel.bitwidth=ADC_BITWIDTH_12;
  ESP_ERROR_CHECK(adc_oneshot_config_channel(keyAdc,ADC_CHANNEL_5,&channel));
  adc_cali_curve_fitting_config_t calibration{};calibration.unit_id=ADC_UNIT_1;calibration.chan=ADC_CHANNEL_5;
  calibration.atten=ADC_ATTEN_DB_6;calibration.bitwidth=ADC_BITWIDTH_12;
  adc_cali_create_scheme_curve_fitting(&calibration,&keyCalibration);
  // Passive continuity hint: with all keys released, IO6 should follow the
  // IO9 pull-down/up change. Never drive the remote net as an output.
  gpio_set_pull_mode(gpio_num_t(board::keyPullup),GPIO_PULLDOWN_ONLY);delay(8);
  pullupDownMv=keyVoltage(readKeyAdc().raw);
  gpio_set_pull_mode(gpio_num_t(board::keyPullup),GPIO_PULLUP_ONLY);delay(8);
  pullupUpMv=keyVoltage(readKeyAdc().raw);
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
  if(now-last_<5)return;
  last_=now;
  pullupDigital=digitalRead(board::keyPullup);
  auto adc=readKeyAdc();
  ladder_.sample(keyVoltage(adc.raw),adc.raw>=4080,
                 adc.noiseMv,now,c,sink,ctx);
  millivolts=ladder_.filteredMv;rawMillivolts=ladder_.rawMv;noiseMillivolts=ladder_.noiseMv;
  resistance=ladder_.resistance;decoded=ladder_.decoded;
  powerButton_.tick(!digitalRead(board::keyPower),now,bike::Key::Power,c,sink,ctx);
  modeButton_.tick(!digitalRead(board::keyMode),now,bike::Key::M,c,sink,ctx);
}

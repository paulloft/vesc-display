#include "BikeNetwork.h"
#include <WiFi.h>
#include <Update.h>
#include <esp_system.h>
#include <SD.h>
#include <esp_heap_caps.h>
namespace {
const char html[] PROGMEM=R"HTML(<!doctype html><html lang="ru"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Велодисплей</title><style>
body{font:16px system-ui;background:#07111b;color:#e9f4ff;max-width:650px;margin:24px auto;padding:16px}input,button,textarea{font:inherit;background:#132b3c;color:inherit;border:1px solid #3984a5;border-radius:8px;padding:10px}label{display:block;margin:15px 0}input{width:100px;float:right}button{cursor:pointer}pre{white-space:pre-wrap;overflow-wrap:anywhere}small{color:#a7c4d6}</style>
<h1>Велодисплей</h1><pre id="state">Подключение…</pre><h2>Настройки</h2>
<form id="settings"><div id="fields"></div><button>Применить и сохранить</button></form>
<p id="result"></p><h2>Обновление ESP32</h2><small>Выберите firmware.bin. Обновление доступно при остановке и подтверждённом режиме N.</small><form id="ota"><p><input style="float:none;width:95%" type="file" name="firmware" accept=".bin" required></p><button>Обновить</button></form>
<script>let initial=true;async function refresh(){try{let s=await(await fetch('/api/status')).json();document.querySelector('#state').textContent=JSON.stringify(s,null,2);if(initial){for(let f of s.settings_fields){let l=document.createElement('label');l.textContent=f.label+(f.unit?', '+f.unit:'')+' ';let i=document.createElement('input');Object.assign(i,{type:'number',name:f.key,min:f.min,max:f.max,step:f.step,value:s.config[f.key]});if(f.type==='boolean')Object.assign(i,{type:'checkbox',checked:s.config[f.key]});l.append(i);document.querySelector('#fields').append(l)}initial=false}}catch(e){document.querySelector('#state').textContent='Нет связи'}}refresh();setInterval(refresh,2000);
document.querySelector('#settings').onsubmit=async e=>{e.preventDefault();let body={};for(let x of e.target.elements)if(x.name)body[x.name]=x.type==='checkbox'?x.checked:Number(x.value);let r=await fetch('/api/settings',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});document.querySelector('#result').textContent=await r.text()};
document.querySelector('#ota').onsubmit=async e=>{e.preventDefault();let r=await fetch('/update',{method:'POST',body:new FormData(e.target)});document.querySelector('#result').textContent=await r.text()};</script></html>)HTML";
}
bool BikeNetwork::auth(){if(server_.authenticate("admin",password_.c_str()))return true;server_.requestAuthentication();return false;}
void BikeNetwork::begin(GetStatus status,Apply apply,Ready ready,Ready otaReady,Prepare prepare,bool storageAvailable) {
  char suffix[9];snprintf(suffix,sizeof(suffix),"%06X",unsigned(ESP.getEfuseMac()&0xffffff));
  ssid=String("Ebike-")+suffix;
  WiFi.persistent(false);WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  char secret[17];snprintf(secret,sizeof(secret),"%08lx%08lx",static_cast<unsigned long>(esp_random()),static_cast<unsigned long>(esp_random()));password_=secret;
  if(storageAvailable) {
    File saved=SD.open("/wifi-password.txt",FILE_READ);
    String stored=saved?saved.readStringUntil('\n'):String();if(saved)saved.close();stored.trim();
    bool valid=stored.length()==16;
    for(size_t i=0;i<stored.length();i++)if(!isxdigit(stored[i]))valid=false;
    if(valid)password_=stored;
    else {File file=SD.open("/wifi-password.txt","w");if(file){file.println(password_);file.close();}}
  }
  WiFi.onEvent([](WiFiEvent_t event) {
    if(event==ARDUINO_EVENT_WIFI_AP_STACONNECTED)Serial.println("Wi-Fi: client associated");
    if(event==ARDUINO_EVENT_WIFI_AP_STADISCONNECTED)Serial.println("Wi-Fi: client disconnected");
    if(event==ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED)Serial.println("Wi-Fi: client IP assigned");
  });
  WiFi.softAPConfig(IPAddress(192,168,4,1),IPAddress(192,168,4,1),IPAddress(255,255,255,0));
  online=WiFi.softAP(ssid.c_str(),password_.c_str(),1,false,4);
  Serial.printf("Wi-Fi AP: %s; internal free=%u; largest=%u\n",online?"OK":"FAILED",
      unsigned(ESP.getFreeHeap()),unsigned(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)));
  server_.on("/",HTTP_GET,[this](){if(auth())server_.send_P(200,"text/html; charset=utf-8",html);});
  server_.on("/api/status",HTTP_GET,[this,status](){if(auth())server_.send(200,"application/json; charset=utf-8",status());});
  server_.on("/api/settings",HTTP_POST,[this,apply,ready](){
    if(!auth())return;
    if(!ready()){server_.send(409,"text/plain; charset=utf-8","Настройка доступна после остановки в N");return;}
    if(server_.arg("plain").length()>4096){server_.send(413);return;}
    String result;bool ok=apply(server_.arg("plain"),result);server_.send(ok?200:400,"text/plain; charset=utf-8",result);
  });
  server_.on("/update",HTTP_POST,[this](){
    if(!auth())return;
    bool ok=uploadAllowed_&&uploadOk_&&!Update.hasError();
    server_.send(ok?200:409,"text/plain; charset=utf-8",ok?"Готово. Перезагрузка…":"Обновление отклонено или завершилось ошибкой");
    if(ok){delay(200);ESP.restart();}updating=false;
  },[this,otaReady,prepare](){
    auto& upload=server_.upload();
    if(upload.status==UPLOAD_FILE_START) {
      uploadOk_=false;uploadAllowed_=server_.authenticate("admin",password_.c_str())&&otaReady();
      if(!uploadAllowed_)return;
      prepare();updating=true;uploadAllowed_=Update.begin(UPDATE_SIZE_UNKNOWN,U_FLASH);
    } else if(uploadAllowed_&&upload.status==UPLOAD_FILE_WRITE) {
      if(Update.write(upload.buf,upload.currentSize)!=upload.currentSize){Update.abort();uploadAllowed_=false;updating=false;}
    } else if(uploadAllowed_&&upload.status==UPLOAD_FILE_END) {
      uploadOk_=Update.end(true);updating=false;
    } else if(upload.status==UPLOAD_FILE_ABORTED) {Update.abort();updating=false;uploadAllowed_=false;}
    yield();
  });
  server_.onNotFound([this](){if(auth())server_.send(404,"text/plain","Not found");});server_.begin();
}

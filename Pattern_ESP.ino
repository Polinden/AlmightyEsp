
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsync_WiFiManager.h>    
#include <ArduinoOTA.h>  
#include <AsyncHTTPRequest_Generic.h>
#include <NTPClient.h>  
#include <WiFiUdp.h>
#include <GyverTimer.h> 
#include <ArduinoJson.h> 
#include "index.html.h"


#define DEVNAME "MishRelay"
#define OTAPAS "admin"
#define NUMRELAY 2


AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
AsyncHTTPRequest request;
DNSServer dnsServer; 
GTimer myTimer(MS, 5000);
WiFiClientSecure client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);
boolean relayS [2];
int relaySPins [2]={5,4};
struct Reltime{
  int start_h; 
  int stop_h; 
  int start_m; 
  int stop_m; 
};
Reltime relaySTimes [2]={{100,100,100,100},{100,100,100,100}};
int cur_h;
int cur_m;


void setup_OTA(){
  ArduinoOTA.setHostname(DEVNAME);
  ArduinoOTA.setPassword(OTAPAS);
  ArduinoOTA.begin(); 
}



void setup_WiFiManager(){
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, DEVNAME);
  //ESPAsync_wifiManager.resetSettings();  
  ESPAsync_wifiManager.autoConnect(DEVNAME);
  if (WiFi.status() == WL_CONNECTED) { Serial.print(F("C onnected. Local IP: ")); Serial.println(WiFi.localIP()); }
  else { Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status())); }
}


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


void setup_Server(){
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html::HtmlPage);
  });

  webServer.on("/Relay", HTTP_GET, [] (AsyncWebServerRequest *request) {
       String message;
       int num=0;
       boolean st=false;
       if (request->hasParam("number")) {
          message = request->getParam("number")->value();
          num=atoi(message.c_str())-1;  
          if (request->hasParam("on")) {
             message = request->getParam("on")->value();
             if (strcmp(message.c_str(), "true")==0) st=true; else st=false; 
             updateRelay(num, st);
             request->send(200, "text/plain", "OK");
             return;
           }     
       }
       send_mes_WS("data ", "error");
       request->send(200, "text/plain", "Not-OK");
  });


    webServer.on("/Timer", HTTP_GET, [] (AsyncWebServerRequest *request) {
       String message;
       String st;
       int num=0;
       if (request->hasParam("number")) {
          message = request->getParam("number")->value();
          num=atoi(message.c_str())-1;  
          if (request->hasParam("time")) {
             message = request->getParam("time")->value();
             if (request->hasParam("st")) {
                 st = request->getParam("st")->value();     
                 updateTimer(num, message.c_str(), st.c_str());        
                 request->send(200, "text/plain", "OK");
                 return;
             }
           }     
       }
       send_mes_WS("data ", "error");
       request->send(200, "text/plain", "Not-OK");
  });
  
  webServer.onNotFound(notFound);
  webServer.begin();
}

void updateTimer(int n, const char * time, const char * st){
    if (n<0 || n>=NUMRELAY) {send_mes_WS("data ", "error"); return;}
    if (strcmp(st, "start")==0){
         relaySTimes[n].start_h=parseTime(time)/60;
         relaySTimes[n].start_m=parseTime(time)%60;
      }
    else {
         relaySTimes[n].stop_h=parseTime(time)/60;
         relaySTimes[n].stop_m=parseTime(time)%60;
      }
    send_mes_WS("test ", time);
}


void updateRelay(int n, boolean stat){
    if (n<0 || n>=NUMRELAY) {send_mes_WS("data ", "error"); return;}
    relayS[n]=stat;
    digitalWrite(relaySPins[n], stat?1:0);
}


void doHTTPRequest(){
   HTTPClient http;
   http.useHTTP10(true);
   http.addHeader("Content-Type", "application/json");
   client.setInsecure(); 
   client.setTimeout(5000);
   client.connect( "api.covidtracking.com", 443);
   http.begin(client, "api.covidtracking.com", 443, "/v1/us/current.json", true);
   int httpCode = http.GET();
   if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {send_mes_WS("time ", http.getString().substring(0,1000).c_str());}
   else {send_mes_WS("data ", http.errorToString(httpCode).c_str());};
}
    

void doHTTPRequestAsync(){
  static bool requestOpenResult;
  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone)
  {
    requestOpenResult = request.open("GET", "http://worldtimeapi.org/api/timezone/Europe/Kiev.txt");
    if (requestOpenResult){request.send();}
  }
}

void requestCB(void* optParm, AsyncHTTPRequest* request, int readyState) 
{
  if (readyState == readyStateDone) 
  {
    send_mes_WS("data ", request->responseText().c_str());
    request->setDebug(false);
  }
}


void setup_WS(){
  //ws.onEvent(onWsEvent);
  webServer.addHandler(&ws);
}

void send_mes_WS(const char * topic, const char * text){
  char str[2000];
  strcpy(str, topic);
  strcat(str, " : ");
  strcat(str, text);
  ws.textAll(str);
}

void NTP_setup(){
  timeClient.begin();  
}

void getTimePeriodic(){
   if (myTimer.isReady()) {
     cur_h=timeClient.getHours();
     cur_m=timeClient.getMinutes();
     send_mes_WS("time:", timeClient.getFormattedTime().c_str());  
     informListeners();
     checkRelay();
   }
}


void checkRelay(){
     int z=cur_h*60+cur_m;
     for (int i=0; i<NUMRELAY; i++) {
      int x=relaySTimes[i].start_h*60+relaySTimes[i].start_m;
      int y=relaySTimes[i].stop_h*60+relaySTimes[i].stop_m;
      if (z>=y) {updateRelay(i, false); relaySTimes[i].stop_h=100; relaySTimes[i].stop_m=100;}
      else if (z>=x) {updateRelay(i, true); relaySTimes[i].start_h=100; relaySTimes[i].start_m=100;}   
   }
}


void getJsonData(char * json){
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, json);
}


void initRelays(){
   for (int i=0; i<NUMRELAY; i++) {
      relayS[i]=false;
      pinMode(relaySPins[i], OUTPUT);
      digitalWrite(relaySPins[i], 0);
   }
}


int parseTime(const char * c){
   int h=0;
   int m=0;
   char substr [3];
   char * p=strstr(c, ":");
   if(p!=NULL) {
       strncpy(substr, c, (int)(p-c)); 
       h=atoi(substr);
       strncpy(substr, p+1, 2); 
       m=atoi(substr);
       if(strstr(c, "PM") != NULL) h+=12;
   }
   return h*60+m;
}


void informListeners(){
   char buf [200];
   DynamicJsonDocument doc(1024);
   for (int i=0; i<NUMRELAY; i++) { 
      doc["relay"][i]=relayS[i];
   }
   for (int i=0; i<NUMRELAY; i++) { 
      doc["timer"][i]["start_h"]=relaySTimes[i].start_h;
      doc["timer"][i]["start_m"]=relaySTimes[i].start_m;
      doc["timer"][i]["stop_h"]=relaySTimes[i].stop_h;
      doc["timer"][i]["stop_m"]=relaySTimes[i].stop_m;
      doc["time"]["h"]=cur_h;
      doc["time"]["m"]=cur_m;
   }
   serializeJson(doc, buf);
   send_mes_WS("status", buf);
}

void setup()
{
    Serial.begin(115200); while (!Serial); delay(200);
    setup_WiFiManager();
    setup_OTA();
    setup_WS();
    setup_Server();
    NTP_setup(); 
    initRelays(); 
}

void loop() {  
    request.onReadyStateChange(requestCB);
    ArduinoOTA.handle();  
    timeClient.update();
    getTimePeriodic();
}

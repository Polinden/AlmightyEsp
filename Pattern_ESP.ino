
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

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
AsyncHTTPRequest request;
DNSServer dnsServer; 
GTimer myTimer(MS, 5000);
WiFiClientSecure client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);
boolean relayS [4];
int relaySPins [4]={5,4,4,4};

#define DEVNAME "MishRelay"
#define OTAPAS "admin"


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
  
  webServer.onNotFound(notFound);
  webServer.begin();
}


void updateRelay(int n, boolean stat){
    if (n<0 || n>3) {send_mes_WS("data ", "error"); return;}
    relayS[n]=stat;
    digitalWrite(relaySPins[n], stat?1:0);
    char str[20], st[2]; 
    strcpy(str, "relay ");
    itoa(n+1, st, 10);
    strcat(str, st);
    if (stat) strcat(str, " is on."); else strcat(str, " is of.");
    send_mes_WS("data ", str);
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
     send_mes_WS("time:", timeClient.getFormattedTime().c_str());  
     informListeners();
   }
}

void getJsonData(char * json){
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, json);
}


void initRelays(){
   for (int i=0; i<4; i++) {
      relayS[i]=false;
      pinMode(relaySPins[i], OUTPUT);
      digitalWrite(relaySPins[i], 0);
   }
}


void informListeners(){
   char buf [50];
   DynamicJsonDocument doc(1024);
   for (int i=0; i<4; i++) { 
      doc["relay"][i]=relayS[i];
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

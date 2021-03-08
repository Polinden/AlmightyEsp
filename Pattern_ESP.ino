
#include <ESPAsync_WiFiManager.h>    
#include <ArduinoOTA.h>  
#include <NTPClient.h>  
#include <WiFiUdp.h>
#include <GyverTimer.h> 
#include "index.html.h"
#include "Relay.h"
#include "MQTT.h"
//#include "RHelper.h"


#define DEVNAME "MishRelay"
#define OTAPAS "admin"
#define PINS_NUM 2
#define PINS_AR {5,4}
#define NTPSERV "europe.pool.ntp.org"
#define MQTTSERV "192.168.77.30"

WiFiUDP ntpUDP;
NTPClient * timeClient=NULL;
AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer; 
RelayTimer * myRelays=NULL;
MqtTHelper * myMQTT=NULL;
int pins [] = PINS_AR;
int cur_h;
int cur_m;


GTimer myTimer(MS, 1000);


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
             myRelays->updateRelay(num, st);
             request->send(200, "text/plain", "OK");
             return;
           }     
       }
       send_mes_WS("test ", "error");
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
                 myRelays->updateTimer(num, message.c_str(), st.c_str());        
                 request->send(200, "text/plain", "OK");
                 return;
             }
           }     
       }
       send_mes_WS("test ", "error");
       request->send(200, "text/plain", "Not-OK");
  });
  
  webServer.onNotFound(notFound);
  webServer.begin();
}


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void send_mes_WS(const char * topic, const char * text){
  char str[500];
  strcpy(str, topic);
  strcat(str, " : ");
  strcat(str, text);
  ws.textAll(str);
}


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

void setup_WS(){
  //ws.onEvent(onWsEvent);
  webServer.addHandler(&ws);
}

void NTP_setup(){
  timeClient=new NTPClient(ntpUDP, NTPSERV, 7200, 60000); 
  timeClient->begin();  
}

void setup()
{
    Serial.begin(115200); while (!Serial); delay(200);
    setup_WiFiManager();
    setup_OTA();
    setup_WS();
    setup_Server();
    NTP_setup(); 
    myRelays = new RelayTimer(PINS_NUM, pins); 
    myMQTT = new MqtTHelper(MQTTSERV);
    myRelays->addListener(send_mes_WS);
    myRelays->addListener(&MqtTHelper::pubMqttMessage);
}


void getPeriodically(){
   if (myTimer.isReady()) {
     cur_h=timeClient->getHours();
     cur_m=timeClient->getMinutes();
     myRelays->checkRelay(cur_h, cur_m);
     send_mes_WS("time:", timeClient->getFormattedTime().c_str());  
   }
}


void loop() {  
    ArduinoOTA.handle();  
    timeClient->update();
    getPeriodically();
}



#include "settings.h"
#include <ESPAsync_WiFiManager.h>     
#include <ArduinoOTA.h>  
#include <NTPClient.h>  
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <GyverTimer.h> 
#include "index.html.h"
#include "Relay.h"
#include "MQTT.h"
//#include "RHelper.h"


WiFiUDP ntpUDP;
NTPClient * timeClient=NULL;
AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer; 
RelayTimer * myRelays=NULL;
char MQTT_string_ip [20]="0.0.0.0";
#ifdef MQTT_ADD
MqtTHelper * myMQTT=NULL;
#endif
#define ESP_DRD_USE_LITTLEFS  false
#define ESP_DRD_USE_SPIFFS    false
#define ESP_DRD_USE_EEPROM    true
#include <ESP_DoubleResetDetector.h> 
DoubleResetDetector* drd;
int pins [] = PINS_AR;
int cur_h;
int cur_m;
int cur_s;
bool initialConfig = false;


GTimer myTimer1(MS, 1000);
GTimer myTimer2(MS, 3000);


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


void setup_WiFiManager(char * mqtt, size_t len) {
    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
    ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, DEVNAME); 
    if (ESPAsync_wifiManager.WiFi_SSID()=="") initialConfig=true;
    if (drd->detectDoubleReset()) initialConfig=true;
    if (initialConfig) inConfig(&ESPAsync_wifiManager, mqtt, len);
    else { WiFi.mode(WIFI_STA); WiFi.begin(); }
    //ESPAsync_wifiManager.resetSettings();
    Serial.println(ESPAsync_wifiManager.WiFi_SSID());
    Serial.println(ESPAsync_wifiManager.WiFi_Pass());
    #ifdef MQTT_ADD
    for(size_t i=0;i<len;i++) mqtt[i]=(char)EEPROM.read(0x0F+i); 
    mqtt[len]='\0';
    Serial.print("Mqtt -> ");
    Serial.println(mqtt);
    #endif
    WiFi.waitForConnectResult();
    if (WiFi.status() == WL_CONNECTED) { Serial.print(F("Connected. Local IP: ")); Serial.println(WiFi.localIP()); }
    else { Serial.println("Not connected!");}
}

void inConfig(ESPAsync_WiFiManager * ewm, char * mqtt, size_t len){
      ESPAsync_WiFiManager ESPAsync_wifiManager = *ewm;
      #ifdef MQTT_ADD
      ESPAsync_WMParameter MQTT_name ("MQTT_ip_label", "MQTT_ip", mqtt, len+1);
      ESPAsync_wifiManager.addParameter(&MQTT_name);
      #endif
      if (!ESPAsync_wifiManager.startConfigPortal(DEVNAME, "admin")) {
         ESPAsync_wifiManager.resetSettings();
         #ifdef ESP8266
         ESP.reset();
         #else    
         ESP.restart();
         #endif
         delay(6000);
      }
      #ifdef MQTT_ADD
      strcpy(mqtt, MQTT_name.getValue());
      for(size_t i=0;i<len;i++) EEPROM.write(0x0F+i, mqtt[i]); 
      EEPROM.commit(); 
      #endif
}


void setup_WS(){
  //ws.onEvent(onWsEvent);
  webServer.addHandler(&ws);
}


void NTP_setup(){
  timeClient=new NTPClient(ntpUDP, NTPSERV, TIMESHIFT, 60000); 
  timeClient->begin();  
}

void setup()
{
    Serial.begin(115200); 
    while (!Serial); 
    Serial.println("Start the board!");
    EEPROM.begin(512);
    delay(200);
    setup_WiFiManager(MQTT_string_ip,20);
    setup_OTA();
    setup_WS();
    setup_Server();
    NTP_setup(); 
    myRelays = new RelayTimer(PINS_NUM, pins); 
    myRelays->addListener(send_mes_WS);
    #ifdef MQTT_ADD
    myMQTT = new MqtTHelper(MQTT_string_ip, TOPIC_INF, TOPIC_COM);
    myRelays->addListener(&MqtTHelper::pubMqttMessage);
    #endif
}


void getPeriodically(){
   if (myTimer1.isReady()) {
     cur_h=timeClient->getHours();
     cur_m=timeClient->getMinutes();
     cur_s=timeClient->getSeconds();
     myRelays->checkRelay(cur_h, cur_m, cur_s);
   }
   if (myTimer2.isReady()) {   
     #ifdef MQTT_ADD
     send_mes_WS("mqtt:", MqtTHelper::connected?MQTT_string_ip:"disconnected");  
     #endif
   }
}


void loop() {  
    ArduinoOTA.handle();  
    timeClient->update();
    getPeriodically();
    #ifdef MQTT_ADD
    myMQTT->reconnect();
    #endif
    if (drd) drd->loop();
}

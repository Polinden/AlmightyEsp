
#include <AsyncMqttClient.h>
#include "settings.h"
#define MQTT_ADD
#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#define MQTT_ADD

AsyncMqttClient mqttClient;
typedef void (*MListener)(const char *);


class MqtTHelper{
public:
    MqtTHelper(const char* ip, const char* pub="", const char* sub="");
    ~MqtTHelper(); 
    static void pubMqttMessage(const char* topic, const char* payload);
    static void registerLis(MListener lis);
    static void reconnect();
    static bool connected;
private:
    static void callback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    static void onMqttConnect(bool sessionPresent);
    static unsigned long tries;
    static int all_tries;
    static bool published;
    static bool dissabled;
    static char pub_topic [20];
    static char sub_topic [20];
    static MListener mlis;
};


MqtTHelper::MqtTHelper(const char* host, const char* pub, const char* sub)  {
  if (strstr(host, "0.0.0.0")) return;
  auto ip=new IPAddress();
  if (ip->fromString(host)) {
  strncpy(pub_topic, pub, strlen(pub));
  strncpy(sub_topic, sub, strlen(sub));
  mqttClient.onMessage(callback);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setServer(*ip, MQTT_PORT); 
  dissabled=false;
  }
}

MqtTHelper::~MqtTHelper(){
  mqttClient.disconnect(true);
  dissabled=true;
}


void MqtTHelper::reconnect() {
        if (dissabled) return;
        if (all_tries>MAX_TRY) return;
        if (mqttClient.connected()) {
            connected=true;
            return;  
        }
        else connected=false;
        if ((millis()-tries)>DELAY_MQ) {
            mqttClient.connect();
            if (mqttClient.connected()) { 
                 all_tries=0;
                 connected=true;
            }
            else all_tries++;  
            tries=millis();
        } 
}

void MqtTHelper::onMqttConnect(bool sessionPresent) {
        mqttClient.subscribe(sub_topic, 1); 
}

void MqtTHelper::pubMqttMessage(const char* topic, const char* payload){
    if (dissabled) return;
    if (strcmp(topic, pub_topic)!=0) return;
    if (mqttClient.connected()){
       mqttClient.publish(topic, 1, true, payload);
    }
}

void MqtTHelper::callback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  if (mlis) mlis(payload);
}

void MqtTHelper::registerLis(MListener lis) {
  mlis=lis;
}


unsigned long MqtTHelper::tries=millis();
int  MqtTHelper::all_tries=0;
char MqtTHelper::pub_topic [20]="";
char MqtTHelper::sub_topic [20]="";
bool MqtTHelper::dissabled=true;
bool MqtTHelper::connected=false;
bool MqtTHelper::published=false;
MListener MqtTHelper::mlis=NULL;

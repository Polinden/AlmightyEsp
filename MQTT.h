
#include <PubSubClient.h>
#include "settings.h"
#define MQTT_ADD
#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#define MQTT_ADD

WiFiClient espClient;
PubSubClient client(espClient);


class MqtTHelper{
public:
    MqtTHelper(const char* ip, const char* pub="", const char* sub="");
    ~MqtTHelper(); 
    static void pubMqttMessage(const char* topic, const char* payload);
    static void reconnect();
    static unsigned long tries;
    static int all_tries;
    static bool published;
    static bool dissabled;
    static bool connected;
    static char pub_topic [20];
    static char sub_topic [20];
private:
    static void callback(char* topic, byte* payload, unsigned int length);
};


MqtTHelper::MqtTHelper(const char* host, const char* pub, const char* sub)  {
  if (strstr(host, "0.0.0.0")) return;
  auto ip=new IPAddress();
  if (ip->fromString(host)) {
  client.setServer(*ip, 1883);
  client.setSocketTimeout(MQTT_TO);
  client.setCallback(&MqtTHelper::callback);
  strncpy(pub_topic, pub, strlen(pub));
  strncpy(sub_topic, sub, strlen(sub));
  dissabled=false;
  connected=false;
  published=false;
  }
}

MqtTHelper::~MqtTHelper(){
  if (client.connected()) client.disconnect ();
  dissabled=true;
}


void MqtTHelper::reconnect() {
        if (dissabled) return;
        if (all_tries>MAX_TRY) return;
        if (client.connected()) {
            client.loop();
            connected=true;
            return;  
        }
        else connected=false;
        if ((millis()-tries)>DELAY_MQ) {
            String clientId = String(DEVNAME);
            clientId += String(random(0xffff), HEX);
            if (client.connect(clientId.c_str())) {
              all_tries=0;
              connected=true;
              client.subscribe(sub_topic, 1);
            }
            else all_tries++;  
            tries=millis();
        } 
}

void MqtTHelper::pubMqttMessage(const char* topic, const char* payload){
    if (dissabled) return;
    if (strcmp(topic, pub_topic)!=0) return;
    if (connected){
       published=client.publish(topic, payload, true);
    }
}

void MqtTHelper::callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (unsigned int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
}


unsigned long MqtTHelper::tries=millis();
int  MqtTHelper::all_tries=0;
char MqtTHelper::pub_topic [20]="";
char MqtTHelper::sub_topic [20]="";
bool MqtTHelper::dissabled=true;
bool MqtTHelper::connected=false;
bool MqtTHelper::published=false;

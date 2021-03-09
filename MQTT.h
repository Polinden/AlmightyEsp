#include <Ticker.h>
#include <AsyncMqttClient.h>
#define MQTT_PORT 1883
#define MQTT_ADD

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;



class MqtTHelper{

public:
    MqtTHelper(const char* ip);
    ~MqtTHelper(); 
    static void pubMqttMessage(const char* topic, const char* payload);
private:
    static void connectToMqtt();
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total); 
    #ifdef ESP8266
    static void onWifiConnect(const WiFiEventStationModeGotIP& event); {
    #endif
    #ifdef ESP32
    static void WiFiEvent(WiFiEvent_t event); 
    #endif 
};


MqtTHelper::MqtTHelper(const char* host){
  auto ip=new IPAddress();
  if (ip->fromString(host)) {
       mqttClient.onDisconnect(&MqtTHelper::onMqttDisconnect);
       mqttClient.onMessage(&MqtTHelper::onMqttMessage);
       mqttClient.setServer(*ip, MQTT_PORT);
       #ifdef ESP32
       WiFi.onEvent(WiFiEvent);
       #endif
       #ifdef ESP8266
       WiFi.onStationModeGotIP(onWifiConnect);
       #endif
       connectToMqtt();
  }
}

MqtTHelper::~MqtTHelper(){
}

void MqtTHelper::connectToMqtt() {
  if (mqttClient.connected()) return;
  mqttClient.connect();
}

void MqtTHelper::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  if (WiFi.status() == WL_CONNECTED) {
     mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void MqtTHelper::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(properties.qos);
    Serial.print("  dup: ");
    Serial.println(properties.dup);
    Serial.print("  retain: ");
    Serial.println(properties.retain);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
}

void MqtTHelper::pubMqttMessage(const char* topic, const char* payload){
    if (mqttClient.connected()){
       mqttClient.publish(topic, 1, true, payload);
    }
}

#ifdef ESP8266
void MqtTHelper::onWifiConnect(const WiFiEventStationModeGotIP& event) {
  connectToMqtt();
}
#endif
#ifdef ESP32
void MqtTHelper::WiFiEvent(WiFiEvent_t event) {
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        connectToMqtt();
        break;
    default: break;    
    }
}
#endif

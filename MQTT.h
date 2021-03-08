

#include <Ticker.h>
#include <AsyncMqttClient.h>
#define MQTT_PORT 1883

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
};


MqtTHelper::MqtTHelper(const char* host){
  auto ip=new IPAddress();
  if (ip->fromString(host)) {
       mqttClient.onDisconnect(&MqtTHelper::onMqttDisconnect);
       mqttClient.onMessage(&MqtTHelper::onMqttMessage);
       mqttClient.setServer(*ip, MQTT_PORT);
       connectToMqtt();
  }
}

MqtTHelper::~MqtTHelper(){
}

void MqtTHelper::connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void MqtTHelper::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.detach();
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
       mqttClient.publish(topic, 0, true, payload);
    }
}



#define PINS_NUM 2                  //here and bellow
#define PINS_AR {5,4}               //are GPIOs settings
#define DEVNAME "MishRelay"
#define OTAPAS "admin"
#define NTPSERV "europe.pool.ntp.org"
#define MQTTSERV "192.168.77.30"    //max 20 symbols
#define TIMESHIFT 7200
#define TOPIC_INF "mrelay1"         //max 20 symbols
#define TOPIC_COM "mrelay2"         //max 20 symbols

#define MQTT_PORT 1883
#define MAX_TRY 500
#define DELAY_MQ 3000
#define DRD_TIMEOUT 30
#define DRD_ADDRESS 0

#include <ArduinoJson.h>
#include "settings.h"
#define MAXLIS 10
#define RELAY_ADD

struct Reltime{
      int start_h; 
      int stop_h; 
      int start_m; 
      int stop_m; 
};

typedef void (*RListener)(const char *, const char *);

class RelayTimer{

public:
    RelayTimer(int num, int * pins);
    ~RelayTimer();
    int addListener(RListener lis);
    void removeListener(int lis);
    void updateRelay(int n, boolean stat);
    void updateTimer(int n, const char * time, const char * st);
    void checkRelay(int cur_h, int cur_m, boolean with_report);
    void reportListeners(int cur_h, int cur_m);
    
private:
    Reltime * relaySTimes;
    int numRelays=0;
    int numListeners=0;
    RListener * listeners;
    boolean * relayS;
    int * relaySPins; 
    void informListeners(const char * toic, const char * info);
    void initRelays();
    int parseTime(const char * c);
};


#include "Relay_timer.h"


RelayTimer::RelayTimer(int num, int * pins){
   numRelays=num;
   relaySPins = (int *)malloc(num * sizeof(int));
   relayS = (boolean *)malloc(num * sizeof(boolean));
   relaySTimes = (Reltime *) malloc(num * sizeof(Reltime));
   for (int i=0; i<num; i++) {
       relaySTimes[i].stop_h=100; relaySTimes[i].stop_m=100;
       relaySTimes[i].start_h=100; relaySTimes[i].start_m=100;   
   }
   memcpy (relaySPins, pins, num); 
   listeners = (RListener *) malloc(MAXLIS * sizeof(RListener));
   initRelays();
}




RelayTimer::~RelayTimer(){
   delete relaySPins;
   delete relayS;
   delete relaySTimes;
   delete listeners;
}

void RelayTimer::initRelays(){
   for (int i=0; i<numRelays; i++) {
      relayS[i]=false;
      pinMode(relaySPins[i], OUTPUT);
      digitalWrite(relaySPins[i], 0);
   }
}


void RelayTimer:: informListeners(const char * topic, const char * info){
   for (int i=0; i<numListeners; i++) {
      listeners[i](topic, info);
   }
}


void RelayTimer::updateTimer(int n, const char * time, const char * st){
     if (n<0 || n>=numRelays) return;
     if (strcmp(st, "start")==0){
         relaySTimes[n].start_h=parseTime(time)/60;
         relaySTimes[n].start_m=parseTime(time)%60;
      }
     else {
         relaySTimes[n].stop_h=parseTime(time)/60;
         relaySTimes[n].stop_m=parseTime(time)%60;
      }
      informListeners("test ", time);
}


void RelayTimer::updateRelay(int n, boolean stat){
    if (n<0 || n>=numRelays) return;
    relayS[n]=stat;
    digitalWrite(relaySPins[n], stat?1:0);
    char buf [10];
    itoa(n,buf,10);
    strcat(buf, stat?"=on":"=off");
    informListeners("relay", buf);
}



void RelayTimer::checkRelay(int cur_h, int cur_m, boolean with_report=true){
     int z,x,y=0;
     for (int i=0; i<numRelays; i++) {
      z=cur_m+cur_h*60;
      x=relaySTimes[i].start_h*60+relaySTimes[i].start_m;
      y=relaySTimes[i].stop_h*60+relaySTimes[i].stop_m;
      if (cur_h==0 && relaySTimes[i].stop_h==24) z=cur_m+24*60;      
      if (z>=y) {updateRelay(i, false); relaySTimes[i].stop_h=100; relaySTimes[i].stop_m=100;}
      else {
         if (cur_h==0 && relaySTimes[i].start_h==24) z=cur_m+24*60;   
         if (z>=x) {updateRelay(i, true); relaySTimes[i].start_h=100; relaySTimes[i].start_m=100;}   
      }
      if (with_report) reportListeners(cur_h, cur_m);
   }
}


int RelayTimer::parseTime(const char * c){
   int h=0;
   int m=0;
   char substr [3];
   char * p=strstr(c, ":");
   if(p!=NULL) {
       strncpy(substr, c, (size_t)(p-c)); 
       h=atoi(substr);
       strncpy(substr, p+1, 2); 
       m=atoi(substr);
       if(strstr(c, "PM") != NULL) h+=12;
   }
   return h*60+m;
}


void RelayTimer::reportListeners(int cur_h, int cur_m){
   char buf [200];
   DynamicJsonDocument doc(512);
   for (int i=0; i<numRelays; i++) { 
      doc["relay"][i]=relayS[i];
   }
   for (int i=0; i<numRelays; i++) { 
      doc["timer"][i]["start_h"]=relaySTimes[i].start_h;
      doc["timer"][i]["start_m"]=relaySTimes[i].start_m;
      doc["timer"][i]["stop_h"]=relaySTimes[i].stop_h;
      doc["timer"][i]["stop_m"]=relaySTimes[i].stop_m;
      doc["time"]["h"]=cur_h;
      doc["time"]["m"]=cur_m;
   }
   serializeJson(doc, buf);
   informListeners("status", buf);
}


int RelayTimer::addListener(RListener lis){
  if (numListeners==MAXLIS) return -1;
  listeners[numListeners]=lis;
  numListeners++; 
  return  numListeners;
}


void RelayTimer::removeListener(int lis){
  //stab
}

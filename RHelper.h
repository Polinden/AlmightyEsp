

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
#include <AsyncHTTPRequest_Generic.h>


typedef void (*ListenerHTTP) (void*, AsyncHTTPRequest*, int);

class ReqestSHelper {
public:
    ReqestSHelper();
    ~ReqestS();
    int getHTTPSRequest(char * host, int port, char * url, char * dest, size_t len);
    void geRequestAsync(char * url);
    void registerListener(ListenerHTTP);

private:
   AsyncHTTPRequest * request;
   WiFiClientSecure * client;   
};



ReqestSHelper::ReqestS(){
   request=new AsyncHTTPRequest();
   client=new WiFiClientSecure(); 
}

ReqestSHelper::~ReqestSHelper(){    
}
 
int ReqestSHelper::getHTTPSRequest(char * host, int port, char * url, char * dest, size_t len){
       HTTPClient http;
       http.useHTTP10(true);
       client->setInsecure(); 
       client->setTimeout(5000);
       client->connect(host, port);
       http.begin(*client, host, port, url, true);
       int httpCode = http.GET();
       if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) strncpy(dest, http.getString().substring(0,len).c_str(), len);
       return httpCode;
}
    

void ReqestSHelper::getRequestAsync(char * url){
  static bool requestOpenResult;
  if (request->readyState() == readyStateUnsent || request->readyState() == readyStateDone)
  {
    requestOpenResult = request->open("GET", url);
    if (requestOpenResult){request->send();}
  }
}


void ReqestSHelper::registerListener(ListenerHTTP lis){
   request->onReadyStateChange(lis);
}


void ReqestSHelper::postRequest(char * url, char * playload)
{ 
  static bool requestOpenResult;
  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone)
  {      
    requestOpenResult = request.open("POST", (url + playload).c_str() );    
    if (requestOpenResult)
    {
      request.send();
    }
}


/*void listener(void* optParm, AsyncHTTPRequest* request, int readyState) 
{
  if (readyState == readyStateDone) 
  {
    Serial.println(request->responseText().c_str());
    request->setDebug(false);
  }
}
*/

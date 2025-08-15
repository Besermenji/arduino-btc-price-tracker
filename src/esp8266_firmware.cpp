#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "config.h"

#define VERBOSE 0  // set 1 for USB monitor debug, 0 for quiet run

void dbg(const __FlashStringHelper *msg){ if(VERBOSE) Serial.println(msg);}       
void dbg(const String &s){ if(VERBOSE) Serial.println(s);}                        

String inBuf;

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void handleCmd(const String &cmd);
void httpPrice();
void getTime();

void setup(){
  Serial.begin(115200);
  delay(50);
  dbg(F("\n=== ESP8266 debug firmware ==="));

  // Send ready message immediately
  Serial.println(F("ESP8266 Ready"));

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  dbg(F("WiFi: begin ..."));
  WiFi.begin(ssid,password);

  uint32_t t0=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-t0<15000){delay(250);Serial.print('.');}
  Serial.println();

  if(WiFi.status()==WL_CONNECTED){
    dbg(F("WiFi OK"));
    Serial.print(F("IP: "));Serial.println(WiFi.localIP());
    
    // Initialize NTP client
    timeClient.begin();
    timeClient.setTimeOffset(7200); // Belgrade time: UTC+2 (7200 seconds)
    dbg(F("NTP client initialized for Belgrade time (UTC+2)"));
  }else{
    dbg(F("WiFi FAIL"));
  }
}

void loop(){
  while(Serial.available()){
    char c=Serial.read();
    if(c=='\r'||c=='\n'){
      if(inBuf.length()){handleCmd(inBuf);inBuf="";}
    }else inBuf+=c;
  }
  if(WiFi.status()!=WL_CONNECTED) WiFi.reconnect();
}

void handleCmd(const String &cmd){
  dbg(F("CMD <")); dbg(cmd); dbg(F(">"));
  
  if(cmd=="GET")      httpPrice();
  else if(cmd=="TIME") getTime();
  else if(cmd=="PING") Serial.println(F("PONG"));
  else                 Serial.println(F("ERR unknown cmd"));
}

void httpPrice(){
  if(WiFi.status()!=WL_CONNECTED){
    dbg(F("ERR no WiFi"));
    Serial.println(F("{\"error\":\"WiFi not connected\"}"));
    return;
  }
  
  WiFiClientSecure client;client.setInsecure();
  HTTPClient http;
  const char *url="https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd&include_24hr_change=true";
  dbg(F("HTTP begin"));
  
  if(!http.begin(client,url)){
    dbg(F("ERR begin()"));
    Serial.println(F("{\"error\":\"HTTP begin failed\"}"));
    return;
  }
  
  int code=http.GET();
  dbg(F("HTTP code ")); dbg(String(code));
  
  if(code>0){
    if(code==200){
      String payload=http.getString();
      dbg(F("Len ")); dbg(String(payload.length()));
      // Send ONLY JSON to UNO, no debug info
      Serial.println(payload);
    }else{
      // HTTP error - send error response
      String errorJson = "{\"error\":\"HTTP " + String(code) + "\"}";
      Serial.println(errorJson);
      dbg(F("HTTP error code: ")); dbg(String(code));
    }
  }else{
    // Network error - send error response
    Serial.println(F("{\"error\":\"Network request failed\"}"));
    dbg(F("ERR GET failed"));
  }
  
  http.end();
  dbg(F("Free mem ")); dbg(String(ESP.getFreeHeap()));
}

void getTime(){
  if(WiFi.status()!=WL_CONNECTED){
    dbg(F("ERR no WiFi"));
    Serial.println(F("{\"error\":\"WiFi not connected\"}"));
    return;
  }
  
  dbg(F("NTP update"));
  timeClient.update();
  
  if(timeClient.isTimeSet()){
    // Get Belgrade time (UTC+2)
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    
    // Format time as HH:MM (no seconds)
    String timeStr = String(hours < 10 ? "0" : "") + String(hours) + ":" +
                     String(minutes < 10 ? "0" : "") + String(minutes);
    
    dbg(F("Belgrade Time (UTC+2): ")); dbg(timeStr);
    
    // Send time to Arduino in format: {"time":"HH:MM"}
    String timeJson = "{\"time\":\"" + timeStr + "\"}";
    Serial.println(timeJson);
  }else{
    dbg(F("ERR NTP not set"));
    Serial.println(F("{\"error\":\"NTP not synchronized\"}"));
  }
}

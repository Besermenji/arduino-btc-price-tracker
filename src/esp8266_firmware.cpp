#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

#define VERBOSE 0  // set 1 for USB monitor debug, 0 for quiet run

void dbg(const __FlashStringHelper *msg){ if(VERBOSE) Serial.println(msg);}       
void dbg(const String &s){ if(VERBOSE) Serial.println(s);}                        

String inBuf;

void handleCmd(const String &cmd);
void httpPrice();

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
  else if(cmd=="PING") Serial.println(F("PONG"));
  else if(cmd=="TEST") Serial.println(F("ESP8266 TEST RESPONSE"));
  else                 Serial.println(F("ERR unknown cmd"));
}

void httpPrice(){
  if(WiFi.status()!=WL_CONNECTED){dbg(F("ERR no WiFi"));return;}
  WiFiClientSecure client;client.setInsecure();
  HTTPClient http;
  const char *url="https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";
  dbg(F("HTTP begin"));
  if(!http.begin(client,url)){dbg(F("ERR begin()"));return;}
  int code=http.GET();
  dbg(F("HTTP code ")); dbg(String(code));
  if(code>0){
    String payload=http.getString();
    dbg(F("Len ")); dbg(String(payload.length()));
    // Send ONLY JSON to UNO, no debug info
    Serial.println(payload);
  }else dbg(F("ERR GET failed"));
  http.end();
  dbg(F("Free mem ")); dbg(String(ESP.getFreeHeap()));
}

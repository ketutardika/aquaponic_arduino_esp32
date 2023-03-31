#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "helper_function.h"

const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure clientgo;


void setup_googlesheet(){
  setupEEPROM();
};

void loop_googlesheet(){
  String gsInterval = readEEPROM(3000);
  String gsDevId = readEEPROM(3000 + gsInterval.length() + 1);
  String ReadgsInterval = String(gsInterval);
  String ReadgsDevId = String(gsDevId);
  
  clientgo.setInsecure();
  String value_devices_1 = readEEPROM(4000);
  String value_devices_2 = readEEPROM(4000 + value_devices_1.length() + 1);
  String value_devices_3 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + 2);
  String value_devices_4 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + 3);
  String value_devices_5 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + 4);
  String value_devices_6 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + value_devices_5.length() + 5);
  String value_devices_7 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + value_devices_5.length() + value_devices_6.length() + 6);
  value_devices_7.replace(" ", "");
  value_devices_7.replace("\n","");
  
  String request = "GET /macros/s/"+ReadgsDevId+"/exec?";
  request += "temp="+String(value_devices_1)+"&hum="+String(value_devices_2)+"&tds="+String(value_devices_3)+"&";
  request += "tbdt="+String(value_devices_4)+"&wtemp="+String(value_devices_5)+"&ph="+value_devices_6+"&wlevel="+value_devices_7+" HTTP/1.1";

  if (!clientgo.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  clientgo.println(request);
  clientgo.print("Host: ");
  clientgo.println(host);
  clientgo.println("content-type: text/html");
  clientgo.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36");
  clientgo.println("Connection: close\r\n");
  clientgo.stop();
};

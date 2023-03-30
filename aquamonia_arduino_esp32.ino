#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"
#include "read_serial.h"
#include "helper_function.h"
#include "send_googlesheet.h"

const int ledPin = 2;
const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
long unsigned int previousMillis = 0;

const int intervalGs = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
long unsigned int previousMillisGs = 0;

// Define API endpoint and bearer token
const char* apiEndpoint = "http://os.aquamonia.com/api/v1/sensor-auto/4";
const char* bearerToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJhdWQiOiI0IiwianRpIjoiZTNhZmFiNGY4ZWJkNWRmNWU1NDQ4YjExOGQwYWZjZDkxMWNmMDdkODYyN2RiM2ZlMGZmZGIwYjcwMzAxMjUyNGMyZGM0MDE4YzNkODdlYmUiLCJpYXQiOjE2Nzc0ODM3MDIuMTQzNTk2LCJuYmYiOjE2Nzc0ODM3MDIuMTQzNTk5LCJleHAiOjE3MDkwMTk3MDIuMTI5OTQyLCJzdWIiOiIxIiwic2NvcGVzIjpbXX0.rbEx2hSUe0fz2BjEB91ERb8vLNSxxOwm8u9BgQyPIpdiS422F7FAsEqcJaOuIQpzqM5X7LWnaQJTNowigqqYm7R1Mji5da9Kdt7DXJxHfv21193pLYc4jTx14UkM5-r1Yi6NsEALjhtYDSAwB0MCm8002PZpIm1ZYR3-3Vih4IzEaQAxmCrE_iAOKu5MEfF37W6mdLNqr1WirFkaQnQIBCidpv_gT2ORwAW8z-3whr3Ecc1pUCaOnKKBU4UoaEkz3vbj4QodKTDvTjPg1zX27Fm3xwxB0a-198vztHSz-HPQoy2IscnGMKJT9Y19ywzS3O4sQSLlBcNKMzBZYhOWy1gY9iQqcYNyKdLu5J1m6pSS4Ljr7qFI-dy1ihC2ru9-slnaUM1LvnrqxnfSHegvCyQk8-e8hOBqKaTuww_zN9avBTyo8MCgQiq8Swz4icb-T0_v47tur6wZhTdeg021qviAKCJ5DB0ScMGJfPa3Lvi3KmAtDJUTIsU2Gjv_nmqgem2XB1G9VsvbYpjDeMJOy-iMqQbGg2azKLwnpwaUqN4jRpjn17vqwvsWiX6w8id26pseG4Eh2jHydg3CpekDtGyef6mNVadMP_odWUUmmNeugEPBwom9shUgk3r5ohPavMX6l_Q6y1A1c28QYcfgyRX3LRErMmzSm7gIDu-wnPI";

void setup() {
  Serial.begin(9600);
  setupEEPROM();  
  setup_read_serial();
  setup_wifi_manager();  
  setup_esp_server();
  setup_sendData();
  setup_googlesheet();
  Serial.println("Server Start");
  setupLED(ledPin);
  powerOnLED(ledPin); 
  delay(5000);
}

void loop() { 
  handle_esp_server();
  readSerialData();
  readApiAquamonia();
  sendToServerAquamonia();
  sendToGooleSheetMillis();
}

void sendToServerAquamonia(){
  String intervalhour = readEEPROM(0);
  long unsigned int intervalhourResult = intervalhour.toInt();
  long unsigned int intervalTotal = interval * intervalhourResult;

  if (!intervalhourResult || intervalhourResult==1){
    intervalTotal = 60000;
  }

  long unsigned int currentMillis = millis();
  if (currentMillis - previousMillis >= intervalTotal) {
    handle_sendData(); 
    powerOnLED(ledPin);
    delay(100);
    powerOffLED(ledPin);
    delay(100);
    powerOnLED(ledPin);
    previousMillis = currentMillis;
  }  
}

void readApiAquamonia(){
  HTTPClient httpApi;
    
  httpApi.begin(apiEndpoint);
  httpApi.addHeader("Authorization", "Bearer " + String(bearerToken));
    
  int httpResponseCode = httpApi.GET();
    
  if (httpResponseCode > 0) {
    String response = httpApi.getString();
    StaticJsonDocument<96> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    int device_last_value = doc["device_last_value"]; // 0

    if (device_last_value == 1) {
      fishfeeder();
    }
  } else {
    Serial.println("Error on HTTP request");
  }    
  httpApi.end();
  delay(1000);
}

void sendToGooleSheetMillis(){
  String intervalhourGs = readEEPROM(3000);
  long unsigned int intervalhourResultGs = intervalhourGs.toInt();
  long unsigned int intervalTotalGs = intervalGs * intervalhourResultGs;

  if (!intervalhourResultGs || intervalhourResultGs==1){
    intervalTotalGs = 60000;
  }

  long unsigned int currentMillisGs = millis();
  if (currentMillisGs - previousMillisGs >= intervalTotalGs) {
    loop_googlesheet();
    powerOnLED(ledPin);
    delay(100);
    powerOffLED(ledPin);
    delay(100);
    powerOnLED(ledPin);    
    previousMillisGs = currentMillisGs;
  }  
}
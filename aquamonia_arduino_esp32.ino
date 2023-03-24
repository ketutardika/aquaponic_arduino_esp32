#include <Arduino.h>

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
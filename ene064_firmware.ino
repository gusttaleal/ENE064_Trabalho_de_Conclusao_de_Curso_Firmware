/* eslint-disable */
#include <WiFi.h>
#include <HTTPClient.h>

#include <Arduino.h>
#include <ArduinoJson.h>

#include "firmwareConfig.h"
  
const char* ssid = "<SSID>";
const char* password =  "<PASSWORD>";

const int analogInPin = A0;
int sensorValue = 0;       
int outputValue = 0;       

void initSerialConnection(){
    Serial.begin(115200);
    delay(5000);
}

void initWiFiConnection(){
  int numberOfNetworks = WiFi.scanNetworks();
  for(int i =0; i<numberOfNetworks; i++){
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
    Serial.println("-----------------------");
  }

  WiFi.begin(ssid, password); 
  Serial.print("Connecting to WiFi..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); 
    delay(1000);
  }
  
  Serial.println(""); 
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP()); 
}

void tryToPostData(int outputSensorValue) {
  HTTPClient http;   
  
   http.begin(backEndURL);
   http.addHeader("Content-Type", "application/json");

   const int capacity = JSON_OBJECT_SIZE(3);
   StaticJsonDocument<capacity> doc;

  doc["deviceId"] = deviceId;
  doc["recivedData"] = "<DADO/INSTRUÇÃO RECEBIDA NO MCU>";
  doc["transmittedData"] = outputSensorValue;

  JsonObject obj = doc.to<JsonObject>();

  String output;
  Serial.println(output);
  serializeJson(obj, output);
     
  int httpResponseCode = http.POST(output);
   
  if(httpResponseCode>0){
    String response = http.getString();
  
    Serial.println(httpResponseCode);
    Serial.println(response);  
  }  
  else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void setup() {
  initSerialConnection();
  checkConnection();  
}
  
void loop() {
  if(WiFi.status()== WL_CONNECTED){
    sensorValue = analogRead(analogInPin);
    outputValue = map(sensorValue, 0, 1023, 0, 255);

    tryToPOSTData(outputValue);
  }
  else{
    Serial.println("Error in WiFi connection");   
 }
  delay(5000);  
}

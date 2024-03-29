/*  ==================================================
      UNIVERSIDADE FEDERAL DE JUIZ DE FORA
      ENE064 - TRABALHO DE FINAL DE CURSO/DEP ENERGIA
      GUSTAVO LEAL SILVA E SOUZA - 201469055B
    ==================================================  */
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "firmwareConfig.h"

WiFiClient client;

const int analogInPin = A0;
int sensorValue = 0;
int outputValue = 0;

void initSerialConnection()
{
  Serial.begin(115200);
  delay(5000);
}

void initWiFiConnection()
{
  Serial.println("");
  Serial.println("");

  int numberOfNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numberOfNetworks; i++)
  {
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
    Serial.println("-----------------------");
  }

  String ssid = "";
  String password = "";

  Serial.println("");
  Serial.println("Digite o SSID da rede");
  while (ssid == "")
  {
    if (Serial.available())
    {
      ssid = Serial.readStringUntil('\n');
      Serial.println(ssid);
    }
  }
  Serial.println("Digite a senha da rede");
  while (password == "")
  {
    if (Serial.available())
    {
      password = Serial.readStringUntil('\n');
      Serial.println("*****");
    }
  }
  Serial.println("");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");

  int count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
    count++;
    if (count > 10)
    {
      Serial.println("");
      Serial.println("Timeout - Reset NodeMCU");
      while (true)
        ;
    }
  }

  Serial.println("");
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

int getSensorData()
{
  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  return outputValue;
}

void tryToPostData(int outputSensorValue)
{
  HTTPClient http;

  http.begin(client, backEndURL);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<128> doc;

  doc["deviceId"] = deviceId;
  doc["recivedData"] = 0.00;
  doc["transmittedData"] = outputSensorValue;

  String payload;
  serializeJson(doc, payload);
  Serial.print("Payload: ");
  Serial.println(payload);

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0)
  {
    Serial.println("Success on sending POST");
    Serial.print("Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
    Serial.println("");
  }
  else
  {
    Serial.println("Error on sending POST");
    Serial.print("Response Code: ");
    Serial.println(httpResponseCode);
    Serial.println("");
  }

  http.end();
}

void blinkLed()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(blink);
  digitalWrite(LED_BUILTIN, LOW);
  delay(rateLimit);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  initSerialConnection();
  initWiFiConnection();
}

void loop()
{
  while (WiFi.status() == WL_CONNECTED)
  {
    int sensorData = getSensorData();
    tryToPostData(sensorData);
    blinkLed();
  }
  Serial.println("Error in WiFi connection");
}

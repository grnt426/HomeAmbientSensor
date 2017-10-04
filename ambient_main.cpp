#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <TimerManager.h>
#include <MqttClient.h>
#include <WifiHandler.h>
#include "wifi_pass.secret.h"
#include "ambient_main.h"

const char* deviceId = "ambient_alpha";

#define DHTTYPE DHT22     // DHT 22 (AM2302)
#define DHTPIN 2
DHT_Unified dht(DHTPIN, DHTTYPE);

TimerManager timer;

#ifndef WIFI_SECRET
#error("Please create a wifi_pass.secret.h file with wifi credentials. See the project page for informaiton: https://github.com/grnt426/HomeAmbientSensor")
#endif

WifiHandler wifiHandler(WIFI_SSID, WIFI_PASS);
MqttClient mqttClient(deviceId, callback, &wifiHandler, WIFI_SERV);

unsigned int temp;
unsigned int humidity;
unsigned int lightLevel;

char msg[75];

unsigned int wifiAttempts = 0;
unsigned int mqttAttempts = 0;

void setup() {
  Serial.begin(115200);

  Serial.print("Running as ");
  Serial.println(deviceId);

  checkNetworkStatus();

  if(wifiAttempts < 5 && mqttAttempts < 5) {
    pollSensors();
    sendUpdate();
  }
  else {
    Serial.println("Could not connect to server, will try again after sleeping...");
  }

  deepSleep();
}

void loop() {
  // After Deep Sleeping, the ESP8266 resets and begins at setup(). Loop will never be called
}

void checkNetworkStatus() {

  // We need to wait for the wifi to be connected because otherwise we can't broadcast
  // our message over MQTT before returning to a deep sleep. This *should* mean the device
  // passes through this while very quickly after the first connection unless it is dropped
  // while deep sleeping, which shouldn't happen.
  while(wifiHandler.loop() != 1 && wifiAttempts < 5) {
    wifiAttempts += 1;
    Serial.println("Retrying wifi....");
    delay(2000);
  }

  int mqttState;
  mqttClient.bypassWait(1);
  mqttClient.doNotSubscribe(1);
  while( ((mqttState = mqttClient.loop()) < 1 || mqttState > 3) && mqttAttempts < 5) {
    mqttAttempts += 1;
    Serial.println("Retrying MQTT");
    delay(1000);
  }
}

void pollSensors() {
  dht.begin();
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  
  // TODO: In the future, keep retrying until we get a successful reading.
  if(isnan(event.temperature)) {
    Serial.println("Unable to read sensor...");
  }
  else {
    temp = event.temperature;
    Serial.print("Temperature: ");
    Serial.println(temp);
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    humidity = event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.println(humidity);
  }

  lightLevel = analogRead(A0);
  Serial.print("Light Level: ");
  Serial.println(lightLevel);
}

void sendUpdate() {
  snprintf (msg, 75, "{\"temperature\":%d, \"humidity\":%d, \"light\":%d}", temp, humidity, lightLevel);
  mqttClient.publishMessage("ambient/sync/ambient_alpha", msg);
}

void deepSleep() {
  Serial.println("Sleeping...");
  ESP.deepSleep(120e6); // 120e6us = 2minutes
}

void callback(char* topic, byte* payload, unsigned int length) {
  // We should never receive a message....
}


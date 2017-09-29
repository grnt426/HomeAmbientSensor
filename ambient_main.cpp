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
#error("Please create a wifi_pass.secret.h file with wifi credentials. See the project page for informaiton: https://github.com/grnt426/HomeAcDevice")
#endif

WifiHandler wifiHandler(WIFI_SSID, WIFI_PASS);
MqttClient mqttClient(deviceId, callback, &wifiHandler, WIFI_SERV);

unsigned int temp;
unsigned int humidity;

char msg[50];

void setup() {
  Serial.begin(115200);

  Serial.print("Running as ");
  Serial.println(deviceId);

  checkNetworkStatus();

  pollSensors();

  sendUpdate();

  deepSleep();
}

void loop() {
  // After Deep Sleeping, the ESP8266 resets and begins at setup(). Loop will never be called
}

/**
 * TODO: The below should give up after so many tries so as not to drain the battery.
 * At some point, the server should detect the device dropped (or the server/wifi
 * is down anyway), and no amount of retrying will help on short time-scales.
 */
void checkNetworkStatus() {

  // We need to wait for the wifi to be connected because otherwise we can't broadcast
  // our message over MQTT before returning to a deep sleep. This *should* mean the device
  // passes through this while very quickly after the first connection unless it is dropped
  // while deep sleeping, which shouldn't happen.
  while(wifiHandler.loop() != 1) {
    Serial.println("Retrying wifi....");
    delay(2000);
  }

  int mqttState;
  mqttClient.bypassWait(1);
  while((mqttState = mqttClient.loop()) < 1 || mqttState > 3) {
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
    return;
  }
  temp = event.temperature;
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println();

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
    return;
  }
  humidity = event.relative_humidity;
  Serial.print("  Humidity: ");
  Serial.print(humidity);
  Serial.println();
}

void sendUpdate() {
  snprintf (msg, 50, "{\"temperature\":%d,\"humidity\":%d}", temp, humidity);
  mqttClient.publishMessage("ambient_alpha/sensor", msg);
}

void deepSleep() {
  Serial.println("Sleeping...");
  ESP.deepSleep(120e6); // 120e6us = 2minutes
}

void callback(char* topic, byte* payload, unsigned int length) {
  // We should never receive a message....
}


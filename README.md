Ambient Sensor
==============

To sense and report temperature, light, and humidity every 2 minutes off a 2500mAh battery.

Talks to the [HomeServer](https://github.com/grnt426/HomeServer)

Building The Project
======================

To build the project from source, you will need an Arduino IDE.

Likewise, there are some minor steps for the project files themselves.

Dependencies
-------------

You can use the Arduino's builtin library manager to automatically download the below.

* DHT Sensor Library
  * 1.3.0
  * Library for the DHT22 temperature and humidity sensor
* PubSubClient
  * 2.6.0
  * for chatting with an MQTT server
* ArduinoJson
  * 5.11.0
  * For parsing JSON payloads from the HomeServer
* [HomeAutomationLibrary](https://github.com/grnt426/HomeAutomationLibrary)
  * 1.0.0
  * Common features of devices within my home 

Project Setup
-------------

Create a file called wifi_pass.secret.h, which should have something like the below

    #ifndef WIFI_SECRET
    #define WIFI_SECRET
    #define WIFI_SSID "your wifi's SSID"
    #define WIFI_PASS "your wifi's password"
    #define WIFI_SERV "the MQTT server to connect to (IP address/domain name)"
    #endif
    
Notes on the Circuit
====================

Parts List
-----------

* ESP8266 https://www.adafruit.com/product/2471
* USB to TTL Serial Cable https://www.adafruit.com/product/954
* (Lots of) Resistors http://a.co/6ABTFcK 
* Breadboard wires https://www.adafruit.com/product/153
* half-sized breadboard https://www.adafruit.com/product/64


ESP Pin Setup
--------------
Pint A0 is input for the light sensor

Pin #2 is input for the DHT22

Pin 16 is output to the RST pin, which regulates Deep Sleep
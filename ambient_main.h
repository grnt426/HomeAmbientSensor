#ifndef CONTROLLER_MAIN_H
#define CONTROLLER_MAIN_H

void checkNetworkStatus();

void pollSensors();

void sendUpdate();

void deepSleep();

void callback(char* topic, byte* payload, unsigned int length);

#endif

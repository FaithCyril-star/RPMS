#ifndef MQTT_FUNCTIONS_H
#define MQTT_FUNCTIONS_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h> 
#include <time.h>
#include "secrets.h"


void NTPConnect();
void messageReceived(char *topic, byte *payload, unsigned int length);
void connectAWS();
void publishMessage(float temperature, int BPM, int SpO2, int systolicPressure, int diastolicPressure);
PubSubClient& getClient();
#endif

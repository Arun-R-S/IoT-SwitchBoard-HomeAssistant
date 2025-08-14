#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <Arduino.h>

#ifndef GLOBALS_H
#define GLOBALS_H

const char *ssid = "WifiSSID";          // Name of the Wifi
const char *password = "WifiPassword";  // Password of the Wifi

const char *configFilePath = "/config.json";
const char* configFilename = "/config.ini";

//adminPage Credentials
const char* adminUsername = "adminUsername";  // User name for the Web Portal
const char* adminPassword = "adminPassword";  // Password for the Web Portal

const char *mqttServer = "192.168.1.100";   // The IP of your MQTT broker
const int mqttPort = 1883;                  // Port no for the MQTT broker
const char *mqttUser = "iotlogin";          // username for the MQTT broker
const char *mqttPassword = "arun2k21";      // Passworf for the MQTT broker

int g_mqttCounterConn = 0;

String DeviceMQTTName = "Switch Board 1 MQTT Client";
String DeviceName = "SwitchBoard_1";
String DeviceUniqueId = "switchboard_esp8266_1";
String DeviceModel = "ESP-01";
String DeviceFirmwareVersion = "1.7.4";
String DeviceFirmwareLastUpdatedOn = "13/08/2025 09:01:47 PM";
String DeviceManufacturer = "Arun R S";
String DeviceHardware = "ESP-01 ESP8266EX";
String DeviceStatusTopic = "homeassistant/mydevices/"+DeviceName+"/availability";
String DeviceStatusIcon = "mdi:access-point-network";
String TemperatureTopic = "homeassistant/mydevices/"+DeviceName+"/temperature";

const char *AP_SSID = DeviceName.c_str();   // AP name
const char *AP_PASSWORD = "APpassword";     // AP password
bool isAPActive = false;                    // Flag to track the state of the access point

bool isWifiConnected = false;               // Flag to track the state of the wifi connection

// Structure to hold switch details
struct SwitchDetails {
  int itemNo;
  String name;
  String id;
  int gpioPin;
  String gpioLabel;
  String payload_on;      // Add payload_on and payload_off members
  String payload_off;
  bool entityStatus;      // Add a state member to keep track of the switch state
  bool inverseOn;
  String SwitchName;
};

// Define the maximum number of switches 
const int MAX_SWITCHES = 2;     // configure the number of switches based on the number of GPIO available in the MCU

// Array to hold switch details
SwitchDetails switches[MAX_SWITCHES] = {
    {1, "Switch1", "switchboard_esp8266_1_sw_1", D0, "D0", "ON", "OFF", false, true, "Gate Front Light"},
    {2, "Switch2", "switchboard_esp8266_1_sw_2", D1, "D1", "ON", "OFF", false, true, "Outdoor Focus Light"}
};
// Define structure to store switch state
struct SwitchState {
  bool state[MAX_SWITCHES];
};

// Define the start address for storing data in EEPROM
const int EEPROM_ADDRESS = 50;

int GLOBAL_mqttRetry = 0;
int GLOBAL_mqttRetryLimit = 5;
WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

WiFiServer telnetServer(23);
WiFiClient telnetClient;

unsigned long lastStatusUpdateTime = 0;
const unsigned long statusUpdateInterval = 30000;     // Update status every 60 seconds

unsigned long lastTemperatureUpdateTime = 0;
const unsigned long temperatureUpdateInterval = 1000; // Update status every 1 seconds

unsigned long lastWifiCheckUpdateTime = 0;
const unsigned long wifiCheckUpdateInterval = 1000;   // Update status every 1 seconds

unsigned long lastDiscoveryUpdateTime = 0;
const unsigned long discoveryUpdateInterval = 60000;  // Update status every 60 seconds

// Constants for the thermistor circuit
const int thermistorPin = A0;                                   // ADC pin connected to the thermistor
const int resistorValue = 10000;                                // Resistor value in ohms (10k)
const float nominalTemperature = 25.0;                          // Nominal temperature for the thermistor (in Celsius)
const float nominalResistance = 10000.0;                        // Nominal resistance of the thermistor at nominal temperature
const String UnitMeasurement = "°C";                            // Unit for the temperature reading
const String TemperatureSensorIcon = "mdi:thermometer-lines";   // Home Assistant icon for the temperature sensor



#endif

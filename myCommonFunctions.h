#include "globals.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#ifndef MYCOMMONFUNCTIONS_H
#define MYCOMMONFUNCTIONS_H

template<typename T>
void mySerialPrintln(const T& data) {
  //Serial.print("Connected terminal ");
  //Serial.println(telnetClient.connected());
  Serial.println(data);  // Print to Serial

  // Print to Telnet if client is connected
  //telnetClient = telnetServer.available();
  if (telnetClient.connected()) {
    telnetClient.println(data);
  }
}

template<typename T>
void mySerialPrint(const T& data) {
  Serial.print(data);  // Print to Serial

  // Print to Telnet if client is connected
  //telnetClient = telnetServer.available();
  if (telnetClient.connected()) {
    telnetClient.print(data);
  }
}
bool readSwitchState(int itemNo) {
  byte stateByte = EEPROM.read(EEPROM_ADDRESS + itemNo);  // Read state from EEPROM
  mySerialPrintln("Item " + String(itemNo) + " - " + stateByte + " at the address " + String(EEPROM_ADDRESS + itemNo));
  return stateByte == 1 ? true : false;  // Convert byte to boolean
}

void writeSwitchState(int itemNo, bool state) {
  byte stateByte = state ? 1 : 0;  // Convert boolean to byte
  mySerialPrintln("Write Item " + String(itemNo) + " - " + stateByte + " at the address " + String(EEPROM_ADDRESS + itemNo));
  EEPROM.write(EEPROM_ADDRESS + itemNo, stateByte);  // Write state to EEPROM
  EEPROM.commit();                                   // Commit changes to EEPROM
}

void SubscribeToSwitchStates() {
  // Subscribe to state topics for all switches
  mySerialPrintln("Subscribing......");
  for (int i = 0; i < MAX_SWITCHES; i++) {
    pubSubClient.subscribe(("homeassistant/mydevices/" + DeviceName + "/" + switches[i].name + "/command").c_str());
    mySerialPrintln("Subcribed to : homeassistant/mydevices/" + DeviceName + "/" + switches[i].name + "/command");
  }
}

float getOnBoardTemperature() {
  // Read the analog value from the thermistor pin
  int rawValue = analogRead(thermistorPin);
  //mySerialPrintln(rawValue);

  // Calculate the resistance of the thermistor
  float resistance = resistorValue * (1023.0 / (float)rawValue - 1.0);
  //mySerialPrintln(resistance);
  // Calculate the temperature using the Steinhart-Hart equation
  float steinhart;
  steinhart = resistance / nominalResistance;        // (R/Ro)
  steinhart = log(steinhart);                        // ln(R/Ro)
  steinhart /= 3950.0;                               // 1/B * ln(R/Ro)
  steinhart += 1.0 / (nominalTemperature + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                       // Invert
  steinhart -= 273.15;                               // Convert to Celsius
  //mySerialPrintln(steinhart);
  return steinhart;
}

bool getConfigFile(File& configFile) {
  configFile = SPIFFS.open("/config.json", "r");

  if (!configFile) {
    mySerialPrintln("Failed to open config file");
    configFile.close();
    return false;
  }
  configFile.close();
  return true;
}

String getValueFromConfig(char* key) {
  File configFile = SPIFFS.open(configFilePath, "r");

  if (!configFile) {
    mySerialPrintln("Failed to open config file");
    configFile.close();
    return "";
  }

  // Parse the JSON data from the file
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, configFile);

  if (error) {
    mySerialPrintln("Failed to parse config file");
    configFile.close();
    return "";
  }

  configFile.close();

  return jsonDoc[key];
}
String getValueFromConfig(String key) {
  File configFile = SPIFFS.open(configFilePath, "r");

  if (!configFile) {
    mySerialPrintln("Failed to open config file");
    configFile.close();
    return "";
  }

  // Parse the JSON data from the file
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, configFile);

  if (error) {
    mySerialPrintln("Failed to parse config file");
    configFile.close();
    return "";
  }

  configFile.close();

  return jsonDoc[key];
}
bool setValueInConfig(const char* key, const char* newValue) {
  File configFile = SPIFFS.open(configFilePath, "r");

  if (!configFile) {
    mySerialPrintln("Failed to open config file");
    configFile.close();
    return false;
  }

  // Parse the JSON data from the file
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, configFile);

  if (error) {
    mySerialPrintln("Failed to parse config file");
    configFile.close();
    return false;
  }
  configFile.close();
  configFile = SPIFFS.open(configFilePath, "w");

  if (!configFile) {
    mySerialPrintln("Failed to open config file for writing");
    configFile.close();
    return false;
  }
  jsonDoc[key] = newValue;
  if (serializeJson(jsonDoc, configFile) == 0) {
    mySerialPrintln("Failed to write to config file");
    configFile.close();
    return false;
  } else {
    mySerialPrintln("Config file updated successfully");
  }
  configFile.close();
  return true;
}

String getWifiStatusString() {
  wl_status_t wifiStatus = WiFi.status();
  String wifiMode;
  switch (wifiStatus) {
    case WL_IDLE_STATUS: wifiMode = "Idle"; break;
    case WL_NO_SSID_AVAIL: wifiMode = "No SSID available"; break;
    case WL_SCAN_COMPLETED: wifiMode = "Scan completed"; break;
    case WL_CONNECTED: wifiMode = "Connected"; break;
    case WL_CONNECT_FAILED: wifiMode = "Connection failed"; break;
    case WL_CONNECTION_LOST: wifiMode = "Connection lost"; break;
    case WL_DISCONNECTED: wifiMode = "Disconnected"; break;
    default: wifiMode = "Unknown";
  }
  return wifiMode;
}



#endif

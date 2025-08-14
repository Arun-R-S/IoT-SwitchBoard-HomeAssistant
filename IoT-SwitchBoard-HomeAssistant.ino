
#include "globals.h"
#include "myCommonFunctions.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoOTA.h>

ESP8266WebServer server(80);


#include "WebServerFunctions.h"



// Function to read a line from a file
String readLineFromFile(File file) {
  String line = "";
  char c;
  while (file.available()) {
    c = file.read();
    if (c == '\n') {
      break;
    }
    line += c;
  }
  return line;
}



// Function to parse INI file and extract value of a specific key in a section
String getValueFromINI(const char *filename, const char *section, const char *key) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    mySerialPrintln("Failed to open file for reading");
    return "";
  }

  String line;
  String sectionName = "[" + String(section) + "]";
  String keyName = String(key) + "=";

  bool foundSection = false;

  // Find the section
  while (file.available()) {
    line = readLineFromFile(file);
    if (line.indexOf(sectionName) != -1) {
      foundSection = true;
      break;
    }
  }

  if (!foundSection) {
    mySerialPrintln("Section not found: " + sectionName);
    file.close();  // Close the file before returning
    return "";
  }

  // Find the key within the section
  while (file.available()) {
    line = readLineFromFile(file);
    if (line.startsWith(keyName)) {
      file.close();  // Close the file before returning
      return line.substring(keyName.length());
    }
  }

  mySerialPrintln("Key not found: " + keyName);
  file.close();  // Close the file before returning
  return "";
}


void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      mySerialPrintln("Connected to WiFi and turning off AP");
      // Turn off the access point when connected to WiFi
      //stopConfigPortal();
      //isWifiConnected=true;
      isAPActive = false;
      WiFi.softAPdisconnect(true);
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      mySerialPrintln("Disconnected from WiFi");
      //isWifiConnected=false;
      // Turn on the access point when disconnected from WiFi
      if (isAPActive == false) {
        //wifiManager.setConfigPortalBlocking(false);
        //wifiManager.startConfigPortal(AP_SSID,AP_PASSWORD);
        mySerialPrintln("AP Turned ON");

        //wifiManager.autoConnect(AP_SSID,AP_PASSWORD);
        // Define routes for handling web interface requests
        //wifiManager.autoConnect("AutoConnectAP");
        WiFi.softAP(AP_SSID, AP_PASSWORD);
        isAPActive = true;
        //isWifiConnected=true;
      } else {
        mySerialPrintln("AP Already ON");
      }
      break;
    default:
      break;
  }
}

void setup() {
  ArduinoOTA.setHostname(getValueFromConfig("DeviceName").c_str());  // Set OTA hostname
  ArduinoOTA.setPassword("arun0966");                                // Set OTA password
  ArduinoOTA.begin();
  //wifiManager.startConfigPortal(AP_SSID,AP_PASSWORD);
  //pinMode(A0,INPUT);
  for (int i = 0; i < MAX_SWITCHES; i++) {
    if (switches[i].inverseOn) {
      digitalWrite(switches[i].gpioPin, HIGH);
      pinMode(switches[i].gpioPin, OUTPUT);

    } else {
      digitalWrite(switches[i].gpioPin, LOW);
      pinMode(switches[i].gpioPin, OUTPUT);
    }
  }
  delay(1000);
  EEPROM.begin(512);


  Serial.begin(115200);

  delay(500);
  //mySerialPrintln("Max Switches",MAX_SWITCHES);


  // Restore switch states
  for (int i = 0; i < MAX_SWITCHES; i++) {
    mySerialPrint("EEPROM ");
    mySerialPrintln(String(switches[i].itemNo) + " " + String(readSwitchState(switches[i].itemNo)));
    switches[i].entityStatus = readSwitchState(switches[i].itemNo);
    if (switches[i].inverseOn) {
      digitalWrite(switches[i].gpioPin, switches[i].entityStatus ? LOW : HIGH);
    } else {
      digitalWrite(switches[i].gpioPin, switches[i].entityStatus ? HIGH : LOW);
    }
  }
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    mySerialPrintln("Failed to mount SPIFFS");
  }
  isAPActive = true;
  initializeTheServer(server);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(getValueFromConfig("wifi_ssid"), getValueFromConfig("wifi_password"));

  //wifiManager.autoConnect(AP_SSID,AP_PASSWORD);
  mySerialPrintln("Connecting to Wi-Fi");

  //WiFi.softAP(AP_SSID,AP_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    mySerialPrint(".");
    server.handleClient();
  }
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  // Open the root directory
  Dir dir = SPIFFS.openDir("/");

  mySerialPrintln("Files stored in SPIFFS:");

  // List all files in the root directory
  while (dir.next()) {
    String fileName = dir.fileName();
    mySerialPrint("  - ");
    mySerialPrintln(fileName);
  }
  /*
    String myssid = getValueFromINI("/config.ini", "wifi", "ssid");
    String mypassword = getValueFromINI("/config.ini", "wifi", "password");
    mySerialPrintln("WiFi SSID: " + myssid);
    mySerialPrintln("WiFi Password: " + mypassword);
  */
  //wifiManager.setConfigPortalBlocking(false);
  //wifiManager.startConfigPortal(AP_SSID,AP_PASSWORD);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  mySerialPrintln("Connected to Wi-Fi : " + WiFi.localIP().toString());
  String mqttHostString = getValueFromConfig("mqttHost");
  mySerialPrintln("MQTT Host: " + mqttHostString);                // Print to serial monitor to check the content
  mySerialPrintln("Length: " + String(mqttHostString.length()));  // Print length to verify

  pubSubClient.setServer(mqttHostString.c_str(), getValueFromConfig("mqttPort").toInt());
  //pubSubClient.setServer(mqttServer, mqttPort);
  pubSubClient.setCallback(callback);  // Set callback function for handling incoming messages

  mySerialPrintln("Connecting to MQTT");
  mySerialPrintln(getValueFromConfig("mqttHost").c_str());
  mySerialPrintln(getValueFromConfig("mqttPort").toInt());
  mySerialPrintln(getValueFromConfig("mqttUsername").c_str());
  mySerialPrintln(getValueFromConfig("mqttPassword").c_str());
  while (!pubSubClient.connected()) {
    server.handleClient();
    mySerialPrint(".");
    if (pubSubClient.connect(DeviceMQTTName.c_str(), getValueFromConfig("mqttUsername").c_str(), getValueFromConfig("mqttPassword").c_str(), ("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), 1, true, "offline")) {
      //if (pubSubClient.connect(DeviceMQTTName.c_str(), mqttUser, mqttPassword, ("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), 1, true, "offline")) {

      mySerialPrintln("Connected to MQTT");
      pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), "online", true);
      pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/temperature").c_str(), String(getOnBoardTemperature()).c_str(), true);
      sendDeviceDetails();
      sendDeviceTemperature();
      // Send switch details for discovery
      for (int i = 0; i < MAX_SWITCHES; i++) {
        sendSwitchDetails(switches[i]);
      }

      SubscribeToSwitchStates();
      pubSubClient.subscribe(String("homeassistant/mydevices/" + DeviceName + "/restart").c_str());

      //client.subscribe((DeviceName).c_str());
      //mySerialPrintln("Subcribed to : "+DeviceName);

    } else {

      mySerialPrintln("failed with state ");
      mySerialPrint(pubSubClient.state());
      delay(200);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    mySerialPrintln("===== Sending Data =====");
    char buffer[10];  // Buffer to hold the string representation of the numeric value

    // Convert the numeric value to a string
    sprintf(buffer, "%d", 0);

    //size_t n = serializeJson(doc, buffer);
    for (int i = 0; i < MAX_SWITCHES; i++) {
      String currentPayloadValue = switches[i].entityStatus ? switches[i].payload_on : switches[i].payload_off;
      //mySerialPrintln("Current Payload - "+String(i)+" - "+currentPayloadValue);
      bool published = pubSubClient.publish(("homeassistant/mydevices/" + DeviceName + "/" + switches[i].name + "/state").c_str(), currentPayloadValue.c_str(), true);
      mySerialPrintln("homeassistant/mydevices/" + DeviceName + "/" + switches[i].name + "/state" + " - " + currentPayloadValue);
      delay(30);
    }
  } else {
    mySerialPrintln("WiFi Disconnected");
  }
}

void MqttReconnect() {
  // Loop until we're reconnected
  while (!pubSubClient.connected() && (g_mqttCounterConn++ < 4)) {
    server.handleClient();
    mySerialPrint("Attempting MQTT connection...");
    // Attempt to connect

    if (pubSubClient.connect(DeviceMQTTName.c_str(), getValueFromConfig("mqttUsername").c_str(), getValueFromConfig("mqttPassword").c_str(), ("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), 1, true, "offline")) {
      mySerialPrintln("connected");
      // Subscribe
      pubSubClient.subscribe("homeassistant/status");
      pubSubClient.subscribe(String("homeassistant/mydevices/" + DeviceName + "/restart").c_str());
      SubscribeToSwitchStates();
      delay(100);
    } else {
      mySerialPrint("failed, rc=");
      mySerialPrint(pubSubClient.state());
      mySerialPrintln(" try again in 1 seconds");
      delay(1000);
    }
  }
  GLOBAL_mqttRetry = GLOBAL_mqttRetry + 1;
  if (GLOBAL_mqttRetry > GLOBAL_mqttRetryLimit) {
    ESP.restart();
  }
  g_mqttCounterConn = 0;
}
void sendDeviceTemperature() {
  String discoveryTopic;
  String payload;
  String strPayload;
  if (pubSubClient.connected()) {
    mySerialPrintln("SEND Temperature DISCOVERY!!!");
    StaticJsonDocument<2048> payload;
    JsonObject device;
    JsonArray identifiers;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Temperature
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    discoveryTopic = "homeassistant/sensor/" + DeviceName + "/BoardTemperature" + "/config";

    payload["name"] = "BoardTemperature";
    payload["uniq_id"] = DeviceUniqueId + "_temp";
    payload["stat_t"] = TemperatureTopic;
    payload["unit_of_meas"] = UnitMeasurement;
    payload["ic"] = TemperatureSensorIcon;
    device = payload.createNestedObject("device");
    device["name"] = DeviceName;
    device["model"] = DeviceModel;
    device["sw_version"] = DeviceFirmwareVersion;
    device["manufacturer"] = DeviceManufacturer;
    identifiers = device.createNestedArray("identifiers");
    identifiers.add(DeviceUniqueId);

    serializeJsonPretty(payload, Serial);
    mySerialPrintln(" ");
    serializeJson(payload, strPayload);
    mySerialPrintln(strPayload);
    mySerialPrint("Status code: ");
    mySerialPrintln(pubSubClient.state());
    bool res = pubSubClient.publish(discoveryTopic.c_str(), strPayload.c_str(), true);
    if (!res) {
      mySerialPrintln("Failed to publish message!");
      mySerialPrint("Error code: ");
      mySerialPrintln(pubSubClient.state());
    } else {
      mySerialPrintln("Message published successfully!");
    }
  }

  delay(300);
}
void sendDeviceDetails() {
  String discoveryTopic;
  String payload;
  String strPayload;
  if (pubSubClient.connected()) {
    mySerialPrintln("SEND HOME ASSISTANT DISCOVERY!!!");
    StaticJsonDocument<2048> payload;
    JsonObject device;
    JsonArray identifiers;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Temperature
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    discoveryTopic = "homeassistant/sensor/" + DeviceName + "/MainBoard" + "/config";

    payload["name"] = "Status";
    payload["uniq_id"] = DeviceUniqueId;
    payload["stat_t"] = DeviceStatusTopic;
    payload["ic"] = DeviceStatusIcon;
    device = payload.createNestedObject("device");
    device["name"] = DeviceName;
    device["model"] = DeviceModel;
    device["sw_version"] = DeviceFirmwareVersion;
    device["manufacturer"] = DeviceManufacturer;
    identifiers = device.createNestedArray("identifiers");
    identifiers.add(DeviceUniqueId);

    // Array to store switch configurations
    /*
      JsonObject switchDetails = payload.createNestedObject("switches");

      // Add each switch to the switches array
      for (int i = 0; i < MAX_SWITCHES; i++) {
        String SwitchObjectName = DeviceName +"_"+switches[i].name;
        JsonObject switchObj = switchDetails.createNestedObject(SwitchObjectName);
        switchObj["name"] = DeviceName +"_"+switches[i].name;
        switchObj["command_topic"] = DeviceName+"/"+switches[i].name + "/command";
        switchObj["state_topic"] = DeviceName+"/"+switches[i].name  + "/state";
        switchObj["payload_on"] = switches[i].payload_on;
        switchObj["payload_off"] = switches[i].payload_off;
        switchObj["unique_id"] = switches[i].id;
      }
    */
    serializeJsonPretty(payload, Serial);
    mySerialPrintln(" ");
    serializeJson(payload, strPayload);
    mySerialPrintln(strPayload);
    mySerialPrint("Status code: ");
    mySerialPrintln(pubSubClient.state());
    bool res = pubSubClient.publish(discoveryTopic.c_str(), strPayload.c_str(), true);
    if (!res) {
      mySerialPrintln("Failed to publish message!");
      mySerialPrint("Error code: ");
      mySerialPrintln(pubSubClient.state());
    } else {
      mySerialPrintln("Message published successfully!");
    }
  }

  delay(300);
}
void sendSwitchDetails(const SwitchDetails &details) {
  mySerialPrintln("Sending details for " + details.name);
  String discoveryTopic = "homeassistant/switch/" + DeviceName + "/" + details.name + "/config";
  StaticJsonDocument<2048> doc;
  String strPayload;

  doc["name"] = DeviceName + "_" + details.name;
  doc["command_topic"] = "homeassistant/mydevices/" + DeviceName + "/" + details.name + "/command";
  doc["state_topic"] = "homeassistant/mydevices/" + DeviceName + "/" + details.name + "/state";
  doc["payload_on"] = details.payload_on;
  doc["payload_off"] = details.payload_off;
  doc["unique_id"] = details.id;


  serializeJson(doc, strPayload);

  pubSubClient.publish(discoveryTopic.c_str(), strPayload.c_str(), true);
}

void loop() {
  WiFiClient telnetClientTemp = telnetServer.available();
  if (telnetClientTemp) {
    telnetClient = telnetClientTemp;
  }

  ArduinoOTA.handle();
  server.handleClient();


  if (WiFi.status() == WL_CONNECTED) {
    if (!pubSubClient.connected())
      MqttReconnect();
    else
      pubSubClient.loop();
  }
  //pubSubClient.loop();

  // Update device availability status
  unsigned long currentMillis = millis();
  if (currentMillis - lastStatusUpdateTime >= statusUpdateInterval) {
    lastStatusUpdateTime = currentMillis;

    // Publish device availability status
    if (pubSubClient.connected()) {
      pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), "online", true);
    } else {
      pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), "offline", true);
    }
  }

  if (currentMillis - lastTemperatureUpdateTime >= temperatureUpdateInterval) {
    lastTemperatureUpdateTime = currentMillis;
    float deviceTemp = getOnBoardTemperature();
    mySerialPrintln("Device Temperature : " + String(deviceTemp) + " °C");
    pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/temperature").c_str(), String(deviceTemp).c_str(), true);
  }
  if (currentMillis - lastWifiCheckUpdateTime >= wifiCheckUpdateInterval) {
    lastWifiCheckUpdateTime = currentMillis;
  }
  /* Send Discovery Details*/
  /*
    if (currentMillis - lastDiscoveryUpdateTime >= discoveryUpdateInterval) {
    lastDiscoveryUpdateTime = currentMillis;
    sendDeviceDetails();
      sendDeviceTemperature();
      // Send switch details for discovery
      for (int i = 0; i < MAX_SWITCHES; i++) {
        sendSwitchDetails(switches[i]);
      }
    }
  */
}
String getWordFromMQTTTopic(String topic, int index) {
  int currentWordIndex = 0;
  int startIndex = 0;

  for (int i = 0; i < topic.length(); i++) {
    if (topic.charAt(i) == '/') {
      if (currentWordIndex == index) {
        return topic.substring(startIndex, i);
      }
      startIndex = i + 1;
      currentWordIndex++;
    }
  }

  // Handle the case when the last word in the topic
  if (currentWordIndex == index) {
    return topic.substring(startIndex);
  }

  // Return an empty string if the index is out of range
  return "";
}
void callback(char *topic, byte *payload, unsigned int thisLength) {
  mySerialPrint("Topic - ");
  mySerialPrintln(topic);
  String topicStr = String(topic);
  String thisDevice = getWordFromMQTTTopic(topicStr, 2);
  String thisTopic = getWordFromMQTTTopic(topicStr, 3);
  if (thisDevice == DeviceName && thisTopic == "restart") {
    mySerialPrintln("Restart Component");
    String restartMessage = String((char *)payload).substring(0, thisLength);
    if (restartMessage == "start") {
      mySerialPrintln("Restarting........................................................");
      pubSubClient.publish(("homeassistant/mydevices/" + String(DeviceName) + "/availability").c_str(), "offline", true);
      delay(500);
      ESP.restart();  // Restart the NodeMCU
    } else {
      mySerialPrintln("No Restart");
    }
    return;
  }
  //mySerialPrintln(payload);
  mySerialPrintln("Length " + String(thisLength));
  // Convert char array to string
  if (thisDevice == DeviceName) {
    String switchName = getWordFromMQTTTopic(topicStr, 3);
    mySerialPrintln(switchName);
    // Find corresponding switch in the array
    SwitchDetails *switchDetail = nullptr;
    for (int i = 0; i < MAX_SWITCHES; i++) {
      if (switches[i].name == switchName) {
        switchDetail = &switches[i];
        break;
      }
    }

    // If switch found in array
    if (switchDetail != nullptr) {
      String message = String((char *)payload).substring(0, thisLength);
      mySerialPrintln("Message - " + message);
      if (message == String(switchDetail->payload_on)) {
        // Turn ON corresponding GPIO pin
        mySerialPrintln("Turning the switch ON");
        switchDetail->entityStatus = true;
        pubSubClient.publish(("homeassistant/mydevices/" + DeviceName + "/" + switchDetail->name + "/state").c_str(), switchDetail->payload_on.c_str(), true);
        if (switchDetail->inverseOn) {
          digitalWrite(switchDetail->gpioPin, LOW);
        } else {
          digitalWrite(switchDetail->gpioPin, HIGH);
        }
        writeSwitchState(switchDetail->itemNo, true);
      } else if (message == String(switchDetail->payload_off)) {
        // Turn OFF corresponding GPIO pin
        mySerialPrintln("Turning the switch OFF");
        switchDetail->entityStatus = false;
        pubSubClient.publish(("homeassistant/mydevices/" + DeviceName + "/" + switchDetail->name + "/state").c_str(), switchDetail->payload_off.c_str(), true);
        if (switchDetail->inverseOn) {
          digitalWrite(switchDetail->gpioPin, HIGH);
        } else {
          digitalWrite(switchDetail->gpioPin, LOW);
        }
        // Update EEPROM with new switch state
        writeSwitchState(switchDetail->itemNo, false);
      }
      // Publish the updated state back to MQTT
      String newState = switchDetail->entityStatus ? switchDetail->payload_on : switchDetail->payload_off;
      mySerialPrintln("New State is " + newState);

      // Print the updated state (for debugging)
      mySerialPrint("State of ");
      mySerialPrint(switchDetail->name);
      mySerialPrint(" is ");
      mySerialPrintln(switchDetail->entityStatus);
    }
  }
}



#ifndef WEBSERVERFUNCTIONS_H
#define WEBSERVERFUNCTIONS_H

void checkAuthentication() {
  if (!server.authenticate(adminUsername, adminPassword)) {
    return server.requestAuthentication();
  }
}

void handleRoot() {
  checkAuthentication();
  // Open the config.html file from SPIFFS
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    mySerialPrintln("Failed to open file");
    file.close();
    return server.send(500, "text/plain", "Internal Server Error - File not found");
  }

  // Read the contents of the file
  String page = "";
  while (file.available()) {
    page += (char)file.read();
  }
  file.close();
  //page.replace("%_DeviceName%", DeviceName);
  // Send the HTML content to the client
  server.send(200, "text/html", page);
}



void handleConfig() {
  //checkAuthentication();
  String myssid = server.arg("ssid");
  String mypassword = server.arg("password");

  if (myssid != "" && mypassword != "") {
    // Connect to WiFi network
    WiFi.begin(myssid.c_str(), mypassword.c_str());
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      mySerialPrint(".");
    }
    mySerialPrintln("");
    mySerialPrintln("WiFi connected");
    mySerialPrintln("IP address: ");
    mySerialPrintln(WiFi.localIP());

    server.send(200, "text/plain", "WiFi configured successfully. Restarting...");
    delay(2000);
    ESP.restart();  // Restart ESP8266
  } else {
    server.send(400, "text/plain", "Invalid SSID or password");
  }
}

void getWifiDetails() {
  StaticJsonDocument<1024> jsonDoc;
  String response;
  // Get the value of a particular key
  jsonDoc["ssid"] = getValueFromConfig("wifi_ssid");
  jsonDoc["password"] = getValueFromConfig("wifi_password");
  jsonDoc["status"] = getWifiStatusString();
  jsonDoc["connectedTo"] = WiFi.SSID();
  jsonDoc["ip"] = WiFi.localIP();
  jsonDoc["mac"] = WiFi.macAddress();
  jsonDoc["signalStrength"] = WiFi.RSSI();
  jsonDoc["firmwareVersion"] = ESP.getFullVersion();
  jsonDoc["apSSID"] = AP_SSID;
  jsonDoc["apPassword"] = AP_PASSWORD;

  serializeJson(jsonDoc, response);
  mySerialPrintln(response);
  server.send(200, "application/json", response);
}
void getStorageDetails() {
  StaticJsonDocument<100> jsonDoc;
  String response;
  jsonDoc["status"] = "success";

  serializeJson(jsonDoc, response);
  mySerialPrintln(response);
  server.send(200, "application/json", response);
}
void getAvailableWifiList() {
  int numNetworks = WiFi.scanNetworks();

  if (numNetworks == 0) {
    mySerialPrintln("No WiFi networks found.");
    server.send(400, "application/json", "No WiFi networks found.");
  } else {
    mySerialPrint("Found ");
    mySerialPrint(numNetworks);
    mySerialPrintln(" WiFi networks:");
    StaticJsonDocument<2048> jsonDoc;
    JsonArray wifiArray = jsonDoc.createNestedArray("networks");

    for (int i = 0; i < numNetworks; ++i) {
      JsonObject wifiObject = wifiArray.createNestedObject();
      wifiObject["ssid"] = WiFi.SSID(i);
      wifiObject["encryptionType"] = WiFi.encryptionType(i);
      wifiObject["rssi"] = WiFi.RSSI(i);
    }

    // Serialize JSON to string
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/json", jsonString);
  }
}
void getSwitchStatusList() {

  StaticJsonDocument<2048> jsonDoc;
  JsonArray thisSwitches = jsonDoc.createNestedArray("switches");

  for (int i = 0; i < MAX_SWITCHES; ++i) {
    JsonObject doc = thisSwitches.createNestedObject();
    doc["name"] = switches[i].SwitchName;
    doc["arrNo"] = i;
    doc["pin"] = switches[i].gpioLabel;
    doc["payloadOn"] = switches[i].payload_on;
    doc["payloadOff"] = switches[i].payload_off;
    doc["inverseSwitch"] = switches[i].inverseOn;
    doc["status"] = digitalRead(switches[i].gpioPin);
  }

  // Serialize JSON to string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  server.send(200, "application/json", jsonString);
}
void getFilesList() {
  Dir dir = SPIFFS.openDir("/");  // Open the root directory

  String fileList = "[";  // Start building the JSON array
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (fileList != "[") {
      fileList += ",";
    }
    fileList += "\"" + String(entry.name()) + "\"";
    entry.close();
  }
  fileList += "]";  // Close the JSON array

  server.send(200, "application/json", fileList);
}
void handleFileUpload() {
  HTTPUpload &upload = server.upload();
  mySerialPrintln(upload.status);
  mySerialPrintln(UPLOAD_FILE_START);
  String filename = "/" + upload.filename;
  mySerialPrintln("Uploading file: " + filename);
  File file = SPIFFS.open(filename, "r");  // Open file for writing (creates if not exists)
  if (!file) {
    mySerialPrintln("Failed to open file");
  } else {
    if (SPIFFS.remove(filename)) {
      mySerialPrintln("File deleted successfully");
    } else {
      mySerialPrintln("Failed to delete file");
    }
  }
  file.close();
  file = SPIFFS.open(filename, "w");  // Open file for appending
  if (!file) {
    mySerialPrintln("Failed to open file for writing");
  }
  if (file.write(upload.buf, upload.currentSize) != upload.currentSize) {
    mySerialPrintln("Error: Failed to write data to file");
    // Handle error condition
  }
  file.close();

  mySerialPrintln("File upload completed");
  server.send(200, "text/plain", "File uploaded successfully");
}
void updateWifiCred() {
  StaticJsonDocument<150> jsonDoc;
  String response;
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String data1 = server.arg("ssid");
    String data2 = server.arg("password");

    // Do something with the received data
    mySerialPrint("Received ssid: ");
    mySerialPrintln(data1);
    mySerialPrint("Received password: ");
    mySerialPrintln(data2);
    if (setValueInConfig("wifi_ssid", data1.c_str()) && setValueInConfig("wifi_password", data2.c_str())) {
      jsonDoc["status"] = "success";

      serializeJson(jsonDoc, response);
      mySerialPrintln(response);
      server.send(200, "application/json", response);
    } else {
      jsonDoc["status"] = "failure";

      serializeJson(jsonDoc, response);
      mySerialPrintln(response);
      server.send(400, "application/json", response);
    }

  } else {
    jsonDoc["status"] = "failure";

    serializeJson(jsonDoc, response);
    mySerialPrintln(response);
    server.send(400, "application/json", response);
  }
}
void updateMQTTDetails() {
  StaticJsonDocument<250> jsonDoc;
  String response;
  if (server.hasArg("mqttHost") && server.hasArg("mqttPort") && server.hasArg("mqttUsername") && server.hasArg("mqttPassword")) {
    String mqttHost = server.arg("mqttHost");
    String mqttPort = server.arg("mqttPort");
    String mqttUsername = server.arg("mqttUsername");
    String mqttPassword = server.arg("mqttPassword");

    if (setValueInConfig("mqttHost", mqttHost.c_str()) && setValueInConfig("mqttPort", mqttPort.c_str()) && setValueInConfig("mqttUsername", mqttUsername.c_str()) && setValueInConfig("mqttPassword", mqttPassword.c_str())) {
      jsonDoc["status"] = "success";

      serializeJson(jsonDoc, response);
      mySerialPrintln(response);
      server.send(200, "application/json", response);
    } else {
      jsonDoc["status"] = "failure";

      serializeJson(jsonDoc, response);
      mySerialPrintln(response);
      server.send(400, "application/json", response);
    }

  } else {
    jsonDoc["status"] = "failure";

    serializeJson(jsonDoc, response);
    mySerialPrintln(response);
    server.send(400, "application/json", response);
  }
}
void getDeviceDetails() {

  //checkAuthentication();
  StaticJsonDocument<200> doc;
  doc["DeviceName"] = getValueFromConfig("DeviceName");
  doc["DeviceID"] = getValueFromConfig("DeviceUniqueId");
  doc["DeviceFirmwareVersion"] = getValueFromConfig("DeviceFirmwareVersion");
  doc["DeviceFirmwareLastUpdatedOn"] = DeviceFirmwareLastUpdatedOn;
  doc["DeviceManufacturer"] = getValueFromConfig("DeviceManufacturer");
  doc["DeviceHardware"] = getValueFromConfig("DeviceHardware");
  doc["DeviceModel"] = getValueFromConfig("DeviceModel");
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}
void toggleSwitch() {
  //checkAuthentication();
  int arrId = server.arg("id").toInt();
  String message = server.arg("stateValue");

  if (message != "" && arrId < MAX_SWITCHES) {
    SwitchDetails *switchDetail = nullptr;
    switchDetail = &switches[arrId];
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
    server.send(200, "text/plain", "Success");
  } else {
    server.send(400, "text/plain", "Invalid error");
  }
}
void getMQTTDetails() {

  //checkAuthentication();
  StaticJsonDocument<200> doc;
  doc["mqttHost"] = getValueFromConfig("mqttHost");
  doc["mqttPort"] = getValueFromConfig("mqttPort");
  doc["mqttUsername"] = getValueFromConfig("mqttUsername");
  doc["mqttPassword"] = getValueFromConfig("mqttPassword");
  doc["mqttStatus"] = pubSubClient.connected();
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleCSS() {
  // Read the CSS file from SPIFFS
  File file = SPIFFS.open("/styles.css", "r");
  if (!file) {
    mySerialPrintln("Failed to open CSS file");
    file.close();
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Send the CSS content to the client
  server.streamFile(file, "text/css");
  file.close();
}
void handleFileRequest() {
  String filePath = server.uri();

  // Read the requested file from SPIFFS
  File file = SPIFFS.open(filePath, "r");
  if (!file) {
    mySerialPrintln("Failed to open file: " + filePath);
    file.close();
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Determine content type based on file extension
  String contentType = "text/plain";
  if (filePath.endsWith(".html")) {
    contentType = "text/html";
  } else if (filePath.endsWith(".css")) {
    contentType = "text/css";
  } else if (filePath.endsWith(".js")) {
    contentType = "application/javascript";
  }

  // Send the file content to the client
  server.streamFile(file, contentType);
  file.close();
}
void handleBootstrap() {
  // Read the CSS file from SPIFFS
  File file = SPIFFS.open("/bootstrap.min.css", "r");
  if (!file) {
    mySerialPrintln("Failed to open CSS file");
    file.close();
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Send the CSS content to the client
  server.streamFile(file, "text/css");
  file.close();
}

void handleJS() {
  // Read the JS file from SPIFFS
  File file = SPIFFS.open("/script.js", "r");
  if (!file) {
    mySerialPrintln("Failed to open JS file");
    file.close();
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Send the JS content to the client
  server.streamFile(file, "application/javascript");
  file.close();
}
void handleJquery() {
  // Read the JS file from SPIFFS
  File file = SPIFFS.open("/jquery.js", "r");
  if (!file) {
    mySerialPrintln("Failed to open JS file");
    file.close();
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Send the JS content to the client
  server.streamFile(file, "application/javascript");
  file.close();
}

void handleJSONConfig() {
  // Read the JS file from SPIFFS
  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    mySerialPrintln("Failed to open JS file");
    server.send(404, "text/plain", "File not found");
    return;
  }

  // Send the JS content to the client
  server.streamFile(file, "application/json");
  file.close();
}
void restartDevice() {
  StaticJsonDocument<100> jsonDoc;
  String response;
  jsonDoc["status"] = "success";
  serializeJson(jsonDoc, response);
  mySerialPrintln("Restarting ESP.....");
  server.send(200, "application/json", response);
  delay(500);
  ESP.restart();
}
void initializeTheServer(ESP8266WebServer &server) {
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/styles.css", handleCSS);
  server.on("/script.js", handleJS);
  server.on("/bootstrap.min.css", handleBootstrap);
  server.on("/jquery.js", handleJquery);
  server.onNotFound(handleFileRequest);

  server.on("/config", HTTP_POST, handleConfig);
  server.on("/getJsonConfig", HTTP_GET, handleJSONConfig);

  server.on("/api/getDeviceDetails", HTTP_GET, getDeviceDetails);
  server.on("/api/restartDevice", HTTP_GET, restartDevice);

  server.on("/api/getWifiDetails", HTTP_GET, getWifiDetails);
  server.on("/api/getFilesList", HTTP_GET, getFilesList);
  server.on("/api/getMQTTDetails", HTTP_GET, getMQTTDetails);
  server.on("/api/getAvailableWifiList", HTTP_GET, getAvailableWifiList);
  server.on("/api/getSwitchStatusList", HTTP_GET, getSwitchStatusList);

  server.on("/api/updateWifiCred", HTTP_POST, updateWifiCred);
  server.on("/api/uploadFile", HTTP_POST, handleFileUpload);
  server.on("/api/updateMQTTDetails", HTTP_POST, updateMQTTDetails);
  server.on("/api/toggleSwitch", HTTP_POST, toggleSwitch);


  // Start web server
  server.begin();
  server.handleClient();
}


#endif

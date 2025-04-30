#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

String TeamID;
#define bleServerName "ESP32_BLE_A40 "

bool connected = false;
bool connected2 = false;

DynamicJsonDocument JsonDoc(4096);

#define SERVICE_UUID "c7d84d70-dac8-11ed-afa1-0242ac120002"
#define CHARACTERISTIC_UUID "d1815718-dac8-11ed-afa1-0242ac120002"

BLEServer *pServer;
BLECharacteristic characteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                                          BLECharacteristic::PROPERTY_WRITE |
                                                          BLECharacteristic::PROPERTY_NOTIFY);

BLEDescriptor *characteristicDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    connected = true;
    Serial.println("Device connected");
  };
  void onDisconnect(BLEServer *pServer) {
    connected = false;
    Serial.println("Device disconnected");
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string tmpCharacteristic = pCharacteristic->getValue();
    Serial.println(tmpCharacteristic.c_str());
    deserializeJson(JsonDoc, tmpCharacteristic.c_str());
    std::string jsonaction = JsonDoc["action"];

    if (jsonaction.compare("getNetworks") == 0) {
      std::string teamID = JsonDoc["teamId"];
      int n = WiFi.scanNetworks();

      for (int i = 0; i < n; ++i) {
        DynamicJsonDocument networkDoc(4096);
        JsonObject jsonnetwork = networkDoc.to<JsonObject>();

        jsonnetwork["ssid"] = WiFi.SSID(i);
        jsonnetwork["strength"] = WiFi.RSSI(i);
        jsonnetwork["encryption"] = WiFi.encryptionType(i);
        jsonnetwork["teamId"] = teamID;

        TeamID = String(teamID.c_str());
        std::string jsonData;
        serializeJson(networkDoc, jsonData);
        serializeJson(networkDoc, Serial);
        pCharacteristic->setValue(jsonData);
        pCharacteristic->notify();
      }
    } else if (jsonaction.compare("connect") == 0) {
      std::string jsonssid = JsonDoc["ssid"];
      std::string jsonpassword = JsonDoc["password"];
      WiFi.begin(jsonssid.c_str(), jsonpassword.c_str());
      delay(5000);
      if (WiFi.status() == WL_CONNECTED) {
        DynamicJsonDocument connectDoc(4096);
        JsonObject jsonconnect = connectDoc.to<JsonObject>();

        jsonconnect["ssid"] = WiFi.SSID();
        jsonconnect["connected"] = true;
        jsonconnect["teamId"] = TeamID;

        std::string jsonData;
        serializeJson(connectDoc, jsonData);
        serializeJson(connectDoc, Serial);
        pCharacteristic->setValue(jsonData);
        pCharacteristic->notify();
      } else {
        Serial.println("Connection failed");
      }
    } else if (jsonaction.compare("getData") == 0) {
      const char* URL = "http://proiectia.bogdanflorea.ro/api/harry-potter/characters";

      WiFiClient client;
      HTTPClient http;

      http.begin(client, URL);

      int httpResponseCode = http.GET();

      if (httpResponseCode == HTTP_CODE_OK) {
        String jsonResponse = http.getString();
        Serial.println(jsonResponse);

        DynamicJsonDocument getDataDoc(4096);
        deserializeJson(getDataDoc, jsonResponse);

        JsonArray records = getDataDoc.as<JsonArray>();

        for (JsonObject record : records) {
          DynamicJsonDocument recordDoc(4096);
          JsonObject jsonRecord = recordDoc.to<JsonObject>();

          jsonRecord["name"] = record["name"];
          jsonRecord["image"] = record["pictureUrl"]; // Fetch the image URL from the response
          jsonRecord["teamId"] = TeamID;
          jsonRecord["id"] = record["id"];

          std::string jsonData;
          serializeJson(recordDoc, jsonData);
          serializeJson(recordDoc, Serial);
          pCharacteristic->setValue(jsonData);
          pCharacteristic->notify();
        }
      } else {
        Serial.print("HTTP request failed with error code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else if (jsonaction.compare("getDetails") == 0) {
  std::string jsonid = JsonDoc["id"];
  std::string URL = "http://proiectia.bogdanflorea.ro/api/harry-potter/character?id=" + jsonid;

  WiFiClient client;
  HTTPClient http;

  http.begin(client, URL.c_str());

  int httpResponseCode = http.GET();

if (httpResponseCode == HTTP_CODE_OK) {
    String jsonResponse = http.getString();
    Serial.println(jsonResponse);

    DynamicJsonDocument detailsDoc(4096);
    deserializeJson(detailsDoc, jsonResponse);  // Deserialize the HTTP response

    JsonObject record = detailsDoc.as<JsonObject>();  // Get the root object of the response

    // Create the response JSON object
    DynamicJsonDocument responseDoc(4096);
    JsonObject response = responseDoc.to<JsonObject>();

    // Construct the description attribute
    String description = "Name: " + String(record["name"].as<const char*>()) + "\n";
    description += "Species: " + String(record["species"].as<const char*>()) + "\n";
    description += "Gender: " + String(record["gender"].as<const char*>()) + "\n";
    description += "House: " + String(record["house"].as<const char*>()) + "\n";
    description += "Date of Birth: " + String(record["dateOfBirth"].as<const char*>()) + "\n";
    description += "Year of Birth: " + String(record["yearOfBirth"].as<int>()) + "\n";
    description += "Wizard: " + String(record["wizard"].as<bool>() ? "Yes" : "No") + "\n";
    description += "Ancestry: " + String(record["ancestry"].as<const char*>()) + "\n";
    description += "Eye Colour: " + String(record["eyeColour"].as<const char*>()) + "\n";
    description += "Hair Colour: " + String(record["hairColour"].as<const char*>()) + "\n";
    description += "Patronus: " + String(record["patronus"].as<const char*>()) + "\n";
    description += "Hogwarts Student: " + String(record["hogwartsStudent"].as<bool>() ? "Yes" : "No") + "\n";
    description += "Hogwarts Staff: " + String(record["hogwartsStaff"].as<bool>() ? "Yes" : "No") + "\n";
    description += "Actor: " + String(record["actor"].as<const char*>()) + "\n";
    description += "Alive: " + String(record["alive"].as<bool>() ? "Yes" : "No")+ "\n";
    description += "Id: " + String(record["id"].as<int>()) + "\n";

    response["id"] = record["id"];
    response["name"] = record["name"];
    response["image"] = record["pictureUrl"]; // Fetch the image URL from the response
    response["description"] = description;    // Add the constructed description
    response["teamId"] = TeamID;

    std::string jsonData;
    serializeJson(responseDoc, jsonData);
    serializeJson(response, Serial);
    pCharacteristic->setValue(jsonData);
    pCharacteristic->notify();
  } else {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init(bleServerName);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  characteristic.setCallbacks(new MyCharacteristicCallbacks());
  BLEService *bleService = pServer->createService(SERVICE_UUID);

  bleService->addCharacteristic(&characteristic);
  characteristic.addDescriptor(characteristicDescriptor);

  bleService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();

  Serial.println("Waiting for a client connection to notify...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(3000);
}

void loop() {
  if (connected == 0 && connected2 == 1) {
    connected2 = connected;
    delay(500);
    pServer->getAdvertising()->start();
    Serial.println("Resuming advertising...");
  }
  if (connected == 1 && connected2 == 0) {
    connected2 = connected;
  }
}
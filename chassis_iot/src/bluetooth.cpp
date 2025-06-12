#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "bluetooth.h"

char currentCmd = 0;
bool newCmd = false;
BLECharacteristic* pCharacteristic = nullptr;

class CommandeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      currentCmd = value[0];
      newCmd = true;
    }
  }
};

class ServeurCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    Serial.println("Un appareil BLE est connecté !");
  }
  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Un appareil BLE s'est déconnecté.");
  }
};

void setupBluetooth() {
  BLEDevice::init("ESP32Car");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServeurCallback());

  BLEUUID serviceUUID("12345678-1234-1234-1234-1234567890ab");
  BLEUUID charUUID("12345678-1234-1234-1234-1234567890ac");

  BLEService *pService = pServer->createService(serviceUUID);

  pCharacteristic = pService->createCharacteristic(
    charUUID,
    BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new CommandeCallback());
  pService->start();

  delay(100);

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);
  pAdvertising->start();

  Serial.println("BLE prêt. Envoyez une commande : a/r/g/d/s");
}
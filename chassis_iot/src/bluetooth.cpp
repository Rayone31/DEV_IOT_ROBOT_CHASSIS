#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "bluetooth.h"

// Variables globales pour stocker la commande reçue et l'état de réception
char currentCmd = 0;
bool newCmd = false;
BLECharacteristic* pCharacteristic = nullptr;

// Callback appelé quand une donnée est écrite sur la caractéristique BLE
class CommandeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      currentCmd = value[0]; // Stocke la première lettre reçue
      newCmd = true;         // Signale qu'une nouvelle commande est arrivée
    }
  }
};

// Callback appelé lors de la connexion/déconnexion d'un client BLE
class ServeurCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    Serial.println("Un appareil BLE est connecté !");
  }
  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Un appareil BLE s'est déconnecté.");
  }
};

// Fonction d'initialisation du Bluetooth BLE
void setupBluetooth() {
  BLEDevice::init("ESP32Car"); // Initialise le BLE avec le nom "ESP32Car"
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServeurCallback()); // Ajoute le callback de connexion

  BLEUUID serviceUUID("12345678-1234-1234-1234-1234567890ab"); // UUID du service
  BLEUUID charUUID("12345678-1234-1234-1234-1234567890ac");    // UUID de la caractéristique

  BLEService *pService = pServer->createService(serviceUUID);

  // Création de la caractéristique BLE en mode écriture
  pCharacteristic = pService->createCharacteristic(
    charUUID,
    BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new CommandeCallback()); // Ajoute le callback d'écriture
  pService->start();

  delay(100); // Petite pause pour la stabilité

  // Démarre la publicité BLE pour rendre l'ESP32 visible
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);
  pAdvertising->start();

  Serial.println("BLE prêt. Envoyez une commande : a/r/g/d/s");
}
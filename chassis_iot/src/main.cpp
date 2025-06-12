#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Pins moteurs
const int IN1 = 19;
const int IN2 = 18;
const int IN3 = 32;
const int IN4 = 33;

char currentCmd = 0;
bool newCmd = false;

BLECharacteristic* pCharacteristic;

// Fonctions moteurs
void avancer() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void reculer() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void droite() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void gauche() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void stopMoteurs() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
}

// Callback BLE pour la réception des commandes
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

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT);
  stopMoteurs();

  // Initialisation BLE
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

void loop() {
  if (newCmd) {
    newCmd = false;
    switch (currentCmd) {
      case 'a': avancer();      Serial.println("Avancer"); break;
      case 'r': reculer();      Serial.println("Reculer"); break;
      case 'g': gauche();       Serial.println("Gauche"); break;
      case 'd': droite();       Serial.println("Droite"); break;
      case 's': stopMoteurs();  Serial.println("Stop"); break;
      default:  Serial.println("Commande inconnue"); break;
    }
  }
  delay(10);
}

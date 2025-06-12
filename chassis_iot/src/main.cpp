#include <Arduino.h>
#include "motors.h"
#include "bluetooth.h"

void setup() {
  Serial.begin(9600);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  stopMoteurs();

  setupBluetooth();
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
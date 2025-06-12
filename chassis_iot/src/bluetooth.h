#pragma once
#include <BLECharacteristic.h>

extern char currentCmd;
extern bool newCmd;
extern BLECharacteristic* pCharacteristic;

void setupBluetooth();
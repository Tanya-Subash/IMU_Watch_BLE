/*
 * Stream data through BLE.
 */
void initBLE() {
  if (!BLE.begin()) {
//    Serial.println("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("imu watch ble2");
  BLE.setAdvertisedService(olaIMUService);

  // add the characteristic to the service
  olaIMUService.addCharacteristic(acclGyroCharacteristic);
  olaIMUService.addCharacteristic(commCharacteristic);
  
  // add service
  BLE.addService(olaIMUService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  
  // assign event handlers for characteristic
  commCharacteristic.setEventHandler(BLEWritten, commCharacteristicWritten);

  // set the initial value for the characteristic:
  acclGyroCharacteristic.writeValue(data_buffer, packetsize*payloadsize);
  commCharacteristic.writeValue(data_buffer, packetsize*payloadsize);

  // start advertising
  BLE.advertise();
//  SERIAL_PORT.println("BLE initialized");
  ble_switched_on = true;
}

bool connectToBLE() {
  central = BLE.central();
  return central.connected();
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
  streaming = false;
}

//void streamBLE() {
//  // listen for BLE centrals to connect:
//  if ((bufferfilled) & (connectToBLE())) {
//    bufferfilled = false;
//    acclGyroCharacteristic.writeValue(data_buffer, packetsize*payloadsize);
//  }
//}

void streamBLE() {
  if (bufferfilled) {
    bufferfilled = false;
    acclGyroCharacteristic.writeValue(data_buffer, packetsize*payloadsize);
  }
}

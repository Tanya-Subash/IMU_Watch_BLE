#include "variables.h"

void setup() {
  SERIAL_PORT.begin(115200);
  initIMU();
  initSD();
  initBLE();
  initInterruptTimer();
  pinMode(11, OUTPUT);
}

void loop() {
  if (not timeset) {
    blink2LED();
    delay(100);
  }
  else {
    readIMU();
  }
  
  // Communication with docking station
  if (isCharging() and (not streaming)) {
    if (timeset) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BLUE_LED, HIGH);
    }
    if (not ble_switched_on) {
      initBLE();
    }
    BLE.poll();
  }
  if (timeset) {
    // Stream real time data
    if (streaming) {
      digitalWrite(RED_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
      BLE.poll();
      streamBLE();
    }
  
    // Log to SD card
    if ((not isCharging()) and (not streaming)) {    
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
      if (isSDPresent()) {
        if (ble_switched_on) {
          BLE.end();
        }
        syncLogFile();
        logSD();
      }
      else {
        // blinks LED when SD card is not present      
        blinkLED(RED_LED);
        delay(500);
      }
    }
  }
}

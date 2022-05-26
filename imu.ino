void initIMU() {
#ifdef USE_SPI
  SPI_PORT.begin();

  pinMode(PIN_IMU_CHIP_SELECT, OUTPUT);
  digitalWrite(PIN_IMU_CHIP_SELECT, HIGH); //Be sure IMU is deselected

  enableCIPOpullUp(); // Enable CIPO pull-up on the OLA

  //There is a quirk in v2.1 of the Apollo3 mbed core which means that the first SPI transaction will
  //disable the pull-up on CIPO. We need to do a fake transaction and then re-enable the pull-up
  //to work around this...
#if defined(ARDUINO_ARCH_MBED)
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); // Do a fake transaction
  SPI.endTransaction();
  enableCIPOpullUp(); // Re-enable the CIPO pull-up
#endif

#else
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
#endif

  //Reset ICM by power cycling it
  imuPowerOff();

  delay(10);

  imuPowerOn(); // Enable power for the OLA IMU

  delay(100); // Wait for the IMU to power up

  bool initialized = false;
  while ( !initialized ) {

#ifdef USE_SPI
    ICM.begin( CS_PIN, SPI_PORT );
#else
    ICM.begin( WIRE_PORT, AD0_VAL );
#endif

//    SERIAL_PORT.print( F("Initialization of the IMU sensor returned: ") );
//    SERIAL_PORT.println( ICM.statusString() );
    if ( ICM.status != ICM_20948_Stat_Ok ) {
//      SERIAL_PORT.println( "Trying again..." );
      delay(500);
    } 
    else {
      initialized = true;
    }
  }
}

void readIMU() {
  if (ICM.dataReady()) {
    ICM.getAGMT();
    readTime();
    _time.data = datetime;
    _imudata.timeaccgyr[0] = ICM.agmt.acc.axes.x;
    _imudata.timeaccgyr[1] = ICM.agmt.acc.axes.y;
    _imudata.timeaccgyr[2] = ICM.agmt.acc.axes.z;
    _imudata.timeaccgyr[3] = ICM.agmt.gyr.axes.x;
    _imudata.timeaccgyr[4] = ICM.agmt.gyr.axes.y;
    _imudata.timeaccgyr[5] = ICM.agmt.gyr.axes.z;
  }
}

void fillDataBuffer(uint8_t buf[packetsize*payloadsize]) {
  int currentbytes = currentsample*packetsize;
  _imudata.timeaccgyr[6] = samplenum;
  memcpy(buf+currentbytes, _time.bytes, sizeof(_time.bytes));
  memcpy(buf+currentbytes+sizeof(_time.bytes), _imudata.bytes, sizeof(_imudata.bytes));
  currentsample += 1;
  if (currentsample >= payloadsize) {
    memcpy(data_buffer, buf, packetsize*payloadsize);
    isbuf1 = !isbuf1;
    bufferfilled = true;
    currentsample = 0;  
  }  
  samplenum += 1;
}

void imuPowerOn() {
  pinMode(PIN_IMU_POWER, OUTPUT);
  digitalWrite(PIN_IMU_POWER, HIGH);
}

void imuPowerOff() {
  pinMode(PIN_IMU_POWER, OUTPUT);
  digitalWrite(PIN_IMU_POWER, LOW);
}

#if defined(ARDUINO_ARCH_MBED) // updated for v2.1.0 of the Apollo3 core
bool enableCIPOpullUp() {
  //Add 1K5 pull-up on CIPO
  am_hal_gpio_pincfg_t cipoPinCfg = g_AM_BSP_GPIO_IOM0_MISO;
  cipoPinCfg.ePullup = AM_HAL_GPIO_PIN_PULLUP_1_5K;
  pin_config(PinName(PIN_SPI_CIPO), cipoPinCfg);
  return (true);
}
#else
bool enableCIPOpullUp() {
  //Add CIPO pull-up
  ap3_err_t retval = AP3_OK;
  am_hal_gpio_pincfg_t cipoPinCfg = AP3_GPIO_DEFAULT_PINCFG;
  cipoPinCfg.uFuncSel = AM_HAL_PIN_6_M0MISO;
  cipoPinCfg.ePullup = AM_HAL_GPIO_PIN_PULLUP_1_5K;
  cipoPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
  cipoPinCfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL;
  cipoPinCfg.uIOMnum = AP3_SPI_IOM;
  padMode(MISO, cipoPinCfg, &retval);
  return (retval == AP3_OK);
}
#endif

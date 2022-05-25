void initSD() {
  // SD.begin should return true if a valid SD card is present
  if ( not isSDPresent()) {
//    SERIAL_PORT.println("SD card not detected.");
  }
  else {
    SERIAL_PORT.println("SD card initialized.");
    openNewLogFile();
  }
}

bool isSDPresent() {
  return sd.begin(PIN_MICROSD_CHIP_SELECT);
}

void openNewLogFile() {
  logFileName = nextLogFile();
  char filename[30];
  logFileName.toCharArray(filename, sizeof(logFileName));
  if (logFile.open(filename, O_CREAT | O_APPEND | O_WRITE) == false) {
    SERIAL_PORT.println("Failed to create sensor data file");
    return;
  }
  updateDataFileCreate(&logFile);
  logFile.sync();
}

void syncLogFile() {
  if (millis() - lastDataLogSyncTime > 500) {
    lastDataLogSyncTime = millis();
    logFile.sync();
    updateDataFileAccess(&logFile);
  }
}

String nextLogFile() {
  String filename;
  int logIndex = 0;

  for (int i = 0; i < LOG_FILE_INDEX_MAX; i++)
  {
    // Construct a file with PREFIX[Index].SUFFIX
    filename = String(LOG_FILE_PREFIX);
    filename += String(logIndex);
    filename += String(LOG_FILE_SUFFIX);
    // If the file name doesn't exist, return it
    if (!sd.exists(filename))
    {
      return filename;
    }
    // Otherwise increment the index, and try again
    logIndex++;
  }

  return "";
}

void logSD() {
  if (bufferfilled) {
    bufferfilled = false;
    if (logFile.size() > (SD_MAX_FILE_SIZE))  {
      logFile.close();
      openNewLogFile();
    }
    if (logFile) {
      logFile.write(data_buffer, payloadsize * packetsize);
    }
  }
}

void updateDataFileCreate(File32 *dataFile) {
  rtc.getTime();
  dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
}

void updateDataFileAccess(File32 *dataFile) {
  rtc.getTime();
  dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
  dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
}

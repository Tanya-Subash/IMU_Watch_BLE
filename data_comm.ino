void commCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  int plSz = commCharacteristic.valueLength();
  const uint8_t* received = commCharacteristic.value();
  char _command = (char) received[0];
  char _message[plSz - 1];
  for (int i = 1; i < plSz; i++) {
    _message[i - 1] = (char) received[i];
  }
  switch (_command) {
    case PING:
      sendCurrentTime();
      break;
    case LISTFILES:
//      listFiles();
      break;
    case SENDFILE:
//      sendFile(_message);
      break;
    case DELETEFILE:
//      deleteFile(readFilename(_message));
      break;
    case SETTIME:
      setTimeOnWatch(_message);
      break;
    case STREAMDATA:
      streaming = true;
      break;
    case STOPSTREAM:
      streaming = false;
      sendCurrentTime();
      break;
    default:
      break;
  }
}

void sendCurrentTime() {
  rtc.getTime();
  timetoble.data[0] = rtc.year;
  timetoble.data[1] = rtc.month;
  timetoble.data[2] = rtc.dayOfMonth;
  timetoble.data[3] = rtc.hour;
  timetoble.data[4] = rtc.minute;
  timetoble.data[5] = rtc.seconds;
  timetoble.data[6] = rtc.hundredths;
  commCharacteristic.writeValue(timetoble.bytes, sizeof(timetoble.bytes));
}


void setTimeOnWatch(char timestring[]) {
  int t[7];
  char *token;
  int i=0;
  token = strtok(timestring, ",");
  while ( token != NULL ) {
    t[i] = String(token).toInt();
    i++;
    token = strtok(NULL, ",");
  }
  rtc.setTime(t[0], t[1], t[2], t[3], t[4], t[5], t[6]); 
  sendCurrentTime();
  timeset = true;
}

//void listFiles()
//{
//  int packetCount = 0, nullFile = 0;
//  if (initSDstream()) 
//  {
//    File32 root = sd.open("/");
//    while (true) 
//    {
//      File32 entry =  root.openNextFile();
//      if (! entry) {
//        /*
//          Null files are 0 byte binary files on the SD card that
//          causes the 'nextfile' pointer to exit the root folder.
//        */
//        nullFile++;
//        packetCount = 0;
//
//        if (nullFile > NULLTHRESHOLD) 
//        {
//          addStringToDataBuffer("EOL");
//          commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//          break;
//        }
//      }
//      String filename_temp = entry.name();
//      if (filename_temp.indexOf(".TXT") > 0) {
//        addStringToDataBuffer(filename_temp);
//      }
//
//      entry.close();
//      packetCount++;
//
//      // Send 10 filenames in one packet
//      if (packetCount > 10) {
//        packetCount = 0;
//        commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//        emptyDataBuffer();
//      }
//    }
//    root.close();
//  }
//}

//void sendFile(char* filename) {
//  // | Line type metadata | Data |
//  if (initSDstream()) {
//    File32 dataFile;
//    dataFile.open(filename, O_CREAT | O_APPEND | O_WRITE);
//    byte buf[lineSz];
//    if (dataFile) {
//      while (dataFile.available()) {
//        if (LOG_PORT.available()) {
//          //  In case there is a command from the docking station to terminate file transfer
//          serReader.readUpdate();
//          if (serReader.payload[0] == TERMINATE) {
//            dataFile.close(); 
//          }
//        }
//        else {
//          // Read from file
//          dataFile.read(buf, lineSz);
//          // Update Out Data Buffer with meta data and data
//          emptyDataBuffer();
//          addByteToDataBuffer(DATALINE);
//          addByteToDataBuffers(buf);
//          commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//        }
//      }
//      //  Reached EOF
//      emptyDataBuffer();
//      addByteToDataBuffer(EOFLINE);
//      commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//      if (dataFile) {
//        digitalWrite(HW_LED_PIN, LOW);
//        dataFile.close();
//      }
//    }
//    else {
//      // File doesn't open
//      emptyDataBuffer();
//      addByteToDataBuffer(ERRORBYTE);
//      addStringToDataBuffer("ERROR WHILE OPENING " + filename);
//      commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//    }
//  }
//}
//
//void deleteFile(String filename) {
//  if (initSDstream()) {
//    emptyDataBuffer();
//    if (sd.remove(filename)) {
//      addByteToDataBuffer(DELETED);
//    }
//    else {
//      addByteToDataBuffer(ERRORBYTE);
//      addStringToDataBuffer("ERROR WHILE DELETING " + filename);
//    }
//    commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//  }
//}
//
//bool initSDstream() {
//  // Initialise SD card for data streaming
//  if (not isSDPresent()) {
//    emptyDataBuffer();
//    addByteToDataBuffer(ERRORBYTE);
//    addStringToDataBuffer("SD CARD ERROR");
//    commCharacteristic.writeValue(dataBuffer, sizeof(dataBuffer));
//    return false;
//  }
//  else {
//    return true;
//  }
//}
//
//String readFilename(char bytearray[]) {
//  // Read filename from command payload from docking station
//  String filename = bytearray;
//  filename.remove(filename.indexOf("TXT") + 3);
//  return filename;
//}

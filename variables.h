/*
 * Header file that contains variable declarations for the IMU watch - OpenLog Artemis program.
 * 
 * Author: Tanya Subash
 * Created on: 12 April 2022
 */
 
#ifndef variables_h
#define variables_h

#include "Arduino.h"
#include <ArduinoBLE.h>
#include "ICM_20948.h"  
#include "RTC.h"
#include <SdFat.h>

//OLA specifics
const byte PIN_IMU_POWER = 27; // The Red SparkFun version of the OLA (V10) uses pin 27
const byte PIN_IMU_INT = 37;
const byte PIN_IMU_CHIP_SELECT = 44;
const byte PIN_SPI_SCK = 5;
const byte PIN_SPI_CIPO = 6;
const byte PIN_SPI_COPI = 7;
const byte PIN_VIN_MONITOR = 34;
const byte PIN_MICROSD_CHIP_SELECT = 23;
const byte BLUE_LED = 19;
const byte RED_LED = 29;

//SD card
SdFat32 sd;
File32 logFile;
int lastDataLogSyncTime = 0;
#define LOG_FILE_INDEX_MAX 999
#define LOG_FILE_PREFIX "log"
#define LOG_FILE_SUFFIX ".txt"
#define SD_MAX_FILE_SIZE 5000000

//Allowable number of 0 byte binary files on the SD card
#define NULLTHRESHOLD 5

//Metadata for sending files
#define DATALINE    0x00
#define EOFLINE     0x01

bool ledState = LOW;
String logFileName = ""; //We keep a record of this file name so that we can re-open it upon wakeup from sleep
bool sdCardPresent = false;


//Commands received from docking station
#define PING        0x01
#define LISTFILES   0x02
#define SENDFILE    0x03
#define DELETEFILE  0x04
#define TERMINATE   0x05
#define SETTIME     0x06
#define STREAMDATA  0x07
#define STOPSTREAM  0x08

//time values - yy, mm, dd, h, m, s, hun; to be sent through BLE
typedef union {
    int data[7];
    uint8_t bytes[28];
} timeunion;

timeunion timetoble;

//BLE
bool streaming = false;
bool ble_switched_on = false;
// maximum bytes per notification is 239+16(uuid)
const int payloadsize = 10; // number of packets 
const int packetsize = 22; // number of bytes in a packet
bool bufferfilled = false;
int currentpacket = 0;

// These UUIDs have been randomly generated. - they must match between the Central and Peripheral devices
// Any changes you make here must be suitably made in the Python program as well

BLEService olaIMUService("13012F00-F8C3-4F4A-A8F4-15CD926DA146"); // BLE Service
// Accelerometer and Gyroscope characteristics
BLECharacteristic acclGyroCharacteristic("13012F01-F8C3-4F4A-A8F4-15CD926DA146", BLENotify , packetsize*payloadsize);
BLECharacteristic commCharacteristic("7B85CD52-6F02-4933-816C-375FB8091A2D", BLERead | BLEWriteWithoutResponse, 200);
BLEDevice central;

/*
 * timer
 */
int prevtime = 0;
int currtime = 0;
int milselapsed = 0;
int logfrequency = 100;
float logratems = 1000 / logfrequency; //(1000 - (payloadsize*30)) / logfrequency;

double datetime = 0;
int prev_datetime = 0;
short mils = 0;
short startmils = 0;
bool interruptflag = false; 
static int interruptTimer = 0;



/*
 * IMU 
 */
 
// IMU data packet
typedef union {
  short timeaccgyr[7];
  uint8_t bytes[14];
} imuunion_t;

typedef union {
    double data;
    uint8_t bytes[8];
} ulongunion_t;

uint8_t data_buffer[packetsize*payloadsize], buf1[packetsize*payloadsize], buf2[packetsize*payloadsize];
bool isbuf1 = true;
uint8_t sd_buffer[packetsize];
ulongunion_t _time;
imuunion_t _imudata;
bool newpacket = false;
bool timeset = false;
bool val = true;

#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI    // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN PIN_IMU_CHIP_SELECT // Which pin you connect CS to. Used only when "USE_SPI" is defined. OLA uses pin 44.

#define WIRE_PORT Wire  // Your desired Wire port.      Used when "USE_SPI" is not defined
#define AD0_VAL   1     // The value of the last bit of the I2C address. 
                        // On the SparkFun 9DoF IMU breakout the default is 1, and when 
                        // the ADR jumper is closed the value becomes 0

#ifdef USE_SPI
  ICM_20948_SPI ICM;  // If using SPI create an ICM_20948_SPI object
#else
  ICM_20948_I2C ICM;  // Otherwise create an ICM_20948_I2C object
#endif


#endif

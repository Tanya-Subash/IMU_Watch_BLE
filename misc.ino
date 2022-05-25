void blinkLED(int LED)
{
  digitalWrite(LED, ledState);
  ledState = !ledState;
}

void blink2LED()
{
  digitalWrite(BLUE_LED, ledState);
  digitalWrite(RED_LED, ledState);
  ledState = !ledState;
}

bool logIntervalElapsed() {
  currtime = millis();
  milselapsed = currtime - prevtime;
  if (milselapsed > logratems) {
    prevtime = currtime;
    return true;
  }
  else {
    return false;
  }
}

void readTime() {
  rtc.getTime();
  datetime = (double) rtc.getEpoch() + (double) rtc.hundredths/100;
}


void getTimeString(char timeStringBuffer[]) {
  //reset the buffer
  timeStringBuffer[0] = '\0';

  rtc.getTime();
  char rtcDate[12]; // 10/12/2019,
  char rtcDay[3];
  char rtcMonth[3];
  char rtcYear[5];
  if (rtc.dayOfMonth < 10)
    sprintf(rtcDay, "0%d", rtc.dayOfMonth);
  else
    sprintf(rtcDay, "%d", rtc.dayOfMonth);
  if (rtc.month < 10)
    sprintf(rtcMonth, "0%d", rtc.month);
  else
    sprintf(rtcMonth, "%d", rtc.month);
  if (rtc.year < 10)
    sprintf(rtcYear, "200%d", rtc.year);
  else
    sprintf(rtcYear, "20%d", rtc.year);
  sprintf(rtcDate, "%s/%s/%s,", rtcYear, rtcMonth, rtcDay);
  strcat(timeStringBuffer, rtcDate);

  char rtcTime[16]; //09:14:37.41, or 09:14:37+00:00,
  int adjustedHour = rtc.hour;
  char rtcHour[3];
  char rtcMin[3];
  char rtcSec[3];
  char rtcHundredths[3];
  char rtcMicros[6];
  char timeZoneH[4];
  char timeZoneM[4];
  if (adjustedHour < 10)
    sprintf(rtcHour, "0%d", adjustedHour);
  else
    sprintf(rtcHour, "%d", adjustedHour);
  if (rtc.minute < 10)
    sprintf(rtcMin, "0%d", rtc.minute);
  else
    sprintf(rtcMin, "%d", rtc.minute);
  if (rtc.seconds < 10)
    sprintf(rtcSec, "0%d", rtc.seconds);
  else
    sprintf(rtcSec, "%d", rtc.seconds);
  if (rtc.hundredths < 10)
    sprintf(rtcHundredths, "0%d", rtc.hundredths);
  else
    sprintf(rtcHundredths, "%d", rtc.hundredths*10);
    
  sprintf(rtcTime, "%s:%s:%s.%s,", rtcHour, rtcMin, rtcSec, rtcHundredths);
  strcat(timeStringBuffer, rtcTime);
}

bool isCharging() {
  int div3 = analogRead(PIN_VIN_MONITOR); //Read VIN across a 1/3 resistor divider
  // 590 corresponds to ~5V
  if (div3 > 590) {
    return true;
  }
  else {
    return false;
  }
}

#include <WiFi.h>
#include <time.h>
#include "SevenSegment.h"

const char *ssid = "DeepSea Developments"; // Name of the Wifi Network
const char *password = "******";           // Password of the Wifi Network

const char *ntpServer = "pool.ntp.org"; // URL of NTP Server
const char *TimeZone = "<-05>5";        // America/Bogota Time Zone
// const char *TimeZone = "PST8PDT,M3.2.0,M11.1.0"; // America/Los_Angeles Time ZOne

#define SDA_PIN 2
#define SCL_PIN 4

// Hour digits
SevenSegment HourTens = SevenSegment(0x40);  // Create an object of Hour Tens driver
SevenSegment HourUnits = SevenSegment(0x40); // Create an object of Hour Units driver

// Minute digits
SevenSegment MinuteTens = SevenSegment(0x41);  // Create an object of Minute Tens driver
SevenSegment MinuteUnits = SevenSegment(0x41); // Create an object of Minute Units driver

// Segments On/Off servo pos                   1    2    3    4    5    6   7
const uint16_t HourTensOnPos[SEGMENT_NUMS] = {375, 375, 390, 350, 375, 390, 340};
const uint16_t HourTensOffPos[SEGMENT_NUMS] = {200, 500, 490, 480, 250, 250, 200};

const uint16_t HourUnitsOnPos[SEGMENT_NUMS] = {375, 375, 390, 350, 368, 390, 340};
const uint16_t HourUnitsOffPos[SEGMENT_NUMS] = {200, 500, 490, 480, 250, 250, 150};

const uint16_t MinuteTensOnPos[SEGMENT_NUMS] = {375, 375, 390, 350, 375, 390, 340};
const uint16_t MinuteTensOffPos[SEGMENT_NUMS] = {200, 500, 490, 480, 250, 250, 200};

const uint16_t MinuteUnitsOnPos[SEGMENT_NUMS] = {375, 375, 390, 350, 368, 390, 340};
const uint16_t MinuteUnitsOffPos[SEGMENT_NUMS] = {200, 500, 490, 480, 250, 250, 150};

void setup()
{
  // Start Serial for debugging
  Serial.begin(115200);

  // Put all the servos ON , 88:88 and wait for ever .Only for Test/Calibration
  CalibrateServos();

  // Start Clock application
  //  InitClock();
}

void loop()
{

  static struct tm prevTimeInfo = {0};

  // Get current time
  struct tm TimeInfo = getCurrentTime();

  // compare minutes , if change then refresh
  if (prevTimeInfo.tm_min != TimeInfo.tm_min)
  {
    prevTimeInfo = TimeInfo;

    // Print Current Time and Date
    Serial.println(&TimeInfo, "%A, %B %d %Y %H:%M:%S");

    // Convert Hour and minutes in Digits
    int Hora = TimeInfo.tm_hour;
    int Mim = TimeInfo.tm_min;

    int hourTens = Hora / 10;
    int hourUnits = Hora % 10;
    int minuteTens = Mim / 10;
    int minuteUnits = Mim % 10;

    // Refresh every servo digit according to the time
    HourTens.setNum(hourTens);
    HourUnits.setNum(hourUnits);
    MinuteTens.setNum(minuteTens);
    MinuteUnits.setNum(minuteUnits);
  }

  // Wait a little  , we don't need to go that fast
  delay(250);
}

// Initialize all servos   , I2C pins and Drivers
void InitServos(void)
{
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!HourTens.begin(HourTensOnPos, HourTensOffPos))
    Serial.println("HourTens not connected!");
  if (!HourUnits.begin(HourUnitsOnPos, HourUnitsOffPos))
    Serial.println("HourUnits not connected!");

  if (!MinuteTens.begin(MinuteTensOnPos, MinuteTensOffPos))
    Serial.println("MinuteTens not connected!");
  if (!MinuteUnits.begin(MinuteUnitsOnPos, MinuteUnitsOffPos))
    Serial.println("MinuteUnits not connected!");
}

// Initialize Clock connecting to Internet and initializing Time and Servos
void InitClock(void)
{

  WiFi.begin(ssid, password);
  // Espera a que se establezca la conexión
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("Connected!");

  initTime(TimeZone);

  InitServos();
}

// Initialize all the servers , by default they always start ON 88:88
void CalibrateServos(void)
{
  InitServos();

  // Wait for ever , so the user can adjust the segment position
  while (1)
    delay(1000);
}

// Initialize time by starting NTP synchronization , wait till get a valid time and set Time Zone
void initTime(String tz)
{
  struct tm TimeInfo;

  Serial.println("Setting up time");
  configTime(0, 0, ntpServer); // First connect to NTP server, with 0 TZ offset
  while (!getLocalTime(&TimeInfo))
  {
    delay(1000);
    Serial.println(".");
  }
  Serial.println("  Got the time from NTP");

  // Now we can set the real timezone
  Serial.printf("  Setting Timezone to %s\n", tz.c_str());
  setenv("TZ", tz.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

// Get current time from internal ESP32 RTC
struct tm getCurrentTime(void)
{
  struct tm TimeInfo;
  if (!getLocalTime(&TimeInfo))
  {
    Serial.println("Invalid Time");
  }
  return TimeInfo;
}

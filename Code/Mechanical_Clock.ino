#include <Adafruit_PWMServoDriver.h>      //Include library for servo driver
#include <WiFi.h>
#include <time.h>


const char *ssid = "Deepsea";       //
const char *password = "Hexaverse"; //

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000; // Offset de GMT en segundos (por ejemplo, -18000 para GMT-5)
const int daylightOffset_sec = 0;   // Offset de horario de verano en segundos

struct tm timeinfo;

#define SDA_PIN 2
#define SCL_PIN 4

Adafruit_PWMServoDriver pwmH = Adafruit_PWMServoDriver(0x40);    //Create an object of Hour driver
Adafruit_PWMServoDriver pwmM = Adafruit_PWMServoDriver(0x41);    //Create an object of Minute driver (A0 Address Jumper)

int servoFrequency = 50;      //Set servo operating frequency

                    //  1   2   3   4   5   6   7    1   2   3   4   5   6   7      //segmentos
int segmentHOn[14] =  {375,375,390,350,375,390,340, 375,375,390,350,368,390,340};   //On positions for each Hour servo
int segmentMOn[14] =  {375,375,390,350,375,390,340, 375,375,390,350,368,390,340};   //On positions for each Minute servo

int segmentHOff[14] = {200,500,490,480,250,250,200, 200,500,490,480,250,250,150};    //Off positions for each Hour servo
int segmentMOff[14] = {200,500,490,480,250,250,200, 200,500,490,480,250,250,150};    //Off positions for each Minute servo

int digits[10][7] = {{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},
                     {1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};    //Position values for each digit

int hourTens = 0;                 //Create variables to store each 7 segment display numeral
int hourUnits = 0;
int minuteTens = 0;
int minuteUnits = 0;

int prevHourTens = 8;           //Create variables to store the previous numeral displayed on each
int prevHourUnits = 8;          //This is required to move the segments adjacent to the middle ones out of the way when they move
int prevMinuteTens = 8;
int prevMinuteUnits = 8;

int midOffset = -100;            //Amount by which adjacent segments to the middle move away when required

void setup() 
{
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  // Espera a que se establezca la conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }
  Serial.println("Conectado a la red WiFi");
  
  // Configura el cliente NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Imprime la hora actual
  printLocalTime();

  Wire.begin(SDA_PIN, SCL_PIN);
  
  pwmH.begin();                             //Start each board
  pwmM.begin();
  pwmH.setOscillatorFrequency(27000000);    //Set the PWM oscillator frequency, used for fine calibration
  pwmM.setOscillatorFrequency(27000000);
  pwmH.setPWMFreq(servoFrequency);          //Set the servo operating frequency
  pwmM.setPWMFreq(servoFrequency);
  
  for(int i=0 ; i<=13 ; i++)    //Set all of the servos to on or up (88:88 displayed)
  {
    Serial.println("All ON");
    pwmH.setPWM(i, 0, segmentHOn[i]);
    delay(10);
    pwmM.setPWM(i, 0, segmentMOn[i]);
    delay(10);
  }
  delay(1000); 
   
  for(int i=0 ; i<=13 ; i++)    //Set all of the servos to off or down ( __:__ displayed)
  {
    Serial.println("All OFF");
    pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(10);
    pwmM.setPWM(i, 0, segmentMOff[i]);
    delay(10);
  }
  delay(1000);

}

void loop()
{
  // Imprime la hora actual
  printLocalTime();

  int Hora = timeinfo.tm_hour;
  int Mim = timeinfo.tm_min;
  int Sec = timeinfo.tm_sec;

  Serial.print("H:");
  Serial.print(Hora);
  Serial.print(" M:");
  Serial.print(Mim);
  Serial.print(" S:");
  Serial.println(Sec);

  // test mecanico o demo, muestra de "segundos" en todos los digitos
  hourTens = Sec / 10;
  hourUnits = Sec % 10;
  minuteTens = Sec / 10;
  minuteUnits = Sec % 10;
    
  // Modo operativo,
  /*
  hourTens = Hora / 10;  
  hourUnits = Hora % 10; 
  minuteTens = Mim / 10; 
  minuteUnits = Mim % 10; 
    */

  if(minuteUnits != prevMinuteUnits){ //If minute units has changed, update display
    updateDisplay();
  }

  prevHourTens = hourTens;            //Update previous displayed numerals
  prevHourUnits = hourUnits;
  prevMinuteTens = minuteTens;
  prevMinuteUnits = minuteUnits;

  delay(200);
}

void printLocalTime() {
  //struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error al obtener la hora");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


void updateDisplay ()                               //Function to update the displayed time
{
  //printLocalTime();

  updateMid();                                      //Move the segments out of the way of the middle segment and then move the middle segments
  for (int i=0 ; i<=5 ; i++)                        //Move the remaining segments
  {
    if(digits[hourTens][i]==1)                      //Update the hour tens
      pwmH.setPWM(i+7, 0, segmentHOn[i+7]);
    else
      pwmH.setPWM(i+7, 0, segmentHOff[i+7]);
    delay(10);
    if(digits[hourUnits][i]==1)                     //Update the hour units
      pwmH.setPWM(i, 0, segmentHOn[i]);
    else
      pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(10);
    if(digits[minuteTens][i]==1)                    //Update the minute tens
      pwmM.setPWM(i+7, 0, segmentMOn[i+7]);
    else
      pwmM.setPWM(i+7, 0, segmentMOff[i+7]);
    delay(10);
    if(digits[minuteUnits][i]==1)                   //Update the minute units
      pwmM.setPWM(i, 0, segmentMOn[i]);
    else
      pwmM.setPWM(i, 0, segmentMOff[i]);
    delay(10);
  }
}

void updateMid()                                              //Function to move the middle segements and adjacent ones out of the way
{
  if(digits[minuteUnits][6]!=digits[prevMinuteUnits][6])      //Move adjacent segments for Minute units
  {
    if(digits[prevMinuteUnits][1]==1)
      pwmM.setPWM(1, 0, segmentMOn[1]-midOffset);
    if(digits[prevMinuteUnits][6]==1)
      pwmM.setPWM(5, 0, segmentMOn[5]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[minuteUnits][6]==1)                               //Move Minute units middle segment if required
    pwmM.setPWM(6, 0, segmentMOn[6]);
  else
    pwmM.setPWM(6, 0, segmentMOff[6]);
  if(digits[minuteTens][6]!=digits[prevMinuteTens][6])        //Move adjacent segments for Minute tens
  {
    if(digits[prevMinuteTens][1]==1)
      pwmM.setPWM(8, 0, segmentMOn[8]-midOffset);
    if(digits[prevMinuteTens][6]==1)
      pwmM.setPWM(12, 0, segmentMOn[12]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[minuteTens][6]==1)                                //Move Minute tens middle segment if required
    pwmM.setPWM(13, 0, segmentMOn[13]);
  else
    pwmM.setPWM(13, 0, segmentMOff[13]);
  if(digits[hourUnits][6]!=digits[prevHourUnits][6])          //Move adjacent segments for Hour units
  {
    if(digits[prevHourUnits][1]==1)
      pwmH.setPWM(1, 0, segmentHOn[1]-midOffset);
    if(digits[prevHourUnits][6]==1)
      pwmH.setPWM(5, 0, segmentHOn[5]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[hourUnits][6]==1)                                 //Move Hour units middle segment if required
    pwmH.setPWM(6, 0, segmentHOn[6]);
  else
    pwmH.setPWM(6, 0, segmentHOff[6]);
  if(digits[hourTens][6]!=digits[prevHourTens][6])            //Move adjacent segments for Hour tens
  {
    if(digits[prevHourTens][1]==1)
      pwmH.setPWM(8, 0, segmentHOn[8]-midOffset);
    if(digits[prevHourTens][6]==1)
      pwmH.setPWM(12, 0, segmentHOn[12]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[hourTens][6]==1)                                  //Move Hour tens middle segment if required
    pwmH.setPWM(13, 0, segmentHOn[13]);
  else
    pwmH.setPWM(13, 0, segmentHOff[13]);
}

//Starter Code for Arduino data logger which contains a realtime clock (RTC) and MicroSD socket.
//For use with the MQ Gas sensors, watch battery will be incorporated to ensure it keeps running, will save display every 5 minutes.
//////////////////////////Hardware Related///////////
///For Data-logger
#include <Wire.h>   //These libraries are to include the use of the RTC and SD modules.
#include <Time.h>
#include "DS1307RTC.h"
#include "RTClib.h"
#include <SdFat.h>
SdFat SD;
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

#define RTC_DS1307 rtc;  //Define the RTC object
char daysofTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define DS20_PIN (SDA20)
#define DS21_PIN (SCL21)
const int chipSelect = 10;
char tmeStrng [6] = ("0000/00/00,00:00:00:");
int logSeconds = 5;  //Number of seconds between logging data.
long logMillis = logSeconds * 1000;
//For Sensors
#define MQ3_PIN (A0)//Analog input of sensor.
#define MQ6_PIN (A1)
#define MQ7_PIN (A2)
#define MQ8_PIN (A3)
#define MQ135_PIN (A4)
#define RL_VALUE (5)//define load resistance in kilo ohms.
#define RO_CLEAN_AIR_FACTOR (9.83)//derived from datasheet.
//////////////////////////Software Related/////////////
#define CALIBRATION_SAMPLE_TIMES (50)//Defining samples in calibration phase.
#define CALIBRATION_SAMPLE_INTERVAL (500)//Time in milliseconds between each sample in calibration.
#define READ_SAMPLE_INTERVAL (50)//How many samples to take in normal operation.
#define READ_SAMPLE_TIMES (1)//Time interval in milliseconds of each sample.
////////////////////////Application Related/////////////////
#define GAS_C2H5OH (0)
#define GAS_CH4    (1)
#define GAS_CO     (2)
#define GAS_H2     (3)
#define GAS_NH3    (4)
#define GAS_CO2    (5)
#define GAS_SECC2H5OH (6)
#define GAS_C6H6   (7)
//////////////////////////////////////////
float           C2H5OHCurve[3] = {7.2,15.1,-0.77};  //Two points are taken from the curve obtained from the datasheet and using web plot digitizer.
                                                    //These two points form a line which approximates the original curve.
                                                    //Data format: {x,y, slope};
float           CH4Curve[3]    =  {7324.5,2.8,-0.48};  
float           COCurve [3]    =  {7110.5,31.9, -0.52};
float           H2Curve [3]    =  {8040.0,28.7,-0.21};    
float           NH3Curve[3]    =  {14.8,7.2,-0.042}; 
float           CO2Curve[3]    =  {26.5,6.9,-0.065};  
float           SECC2H5OHCurve[3] = {7.2,15.1,-0.77};   
float           C6H6Curve[3]   = {194.4,5.39,-0.15}; 
                                                                                                        
float           Ro             =  10;                 
void setup() {
  #ifndef ESP8266
  #endif
  Serial.begin(57600); // baudrate = 9600bps
  Wire.begin();  //Initialise I2C interface.
  RTC.begin();   //Intialise RTC.
  RTC.adjust(DateTime((_DATE_), (_TIME_)));  //This sets the RTC by the time the sketch is compiled.
 {
    ;
  }
  Serial.println ("Data-Logger Calibration Tests");
  Serial.println ("RL_VALUE");
  Serial.println ("RO_CLEAN_AIR_VALUE");
  Serial.print("Find SD card");                     //To intialise the SD card.
  if (!SD.begin(chipselect)) {
    Serial.println("Card failed");
  }
  Serial.println(" SD card OK");
  Serial.print("Logging to microSD card every ");
  Serial.print(logSeconds);
  Serial.println(" seconds.");
  Serial.println();
  File dataFile = SD.open("datalog.txt", FILE_WRITE);   //To print a header data file with columns.
  if (dataFile) {
    dataFile.println("Data-logger Calibration Tests");
    dataFile.print("RL_VALUE");
    dataFile.println("RO_CLEAN_AIR_VALUE");
    datafile.println("Date,Time,UTCtime");
    dataFile.close();
  }
  else {
    Serial.println("file error");                   //If file does not open.
  }
  bool status;
  MQ3_PIN = MQ3.begin(A0);                          //Intialise sensors.
  MQ6_PIN = MQ6.begin(A1);
  MQ7_PIN = MQ7.begin(A2);
  MQ8_PIN = MQ8.begin(A3);
  MQ135_PIN = MQ135.begin(A4);
  if (!status) {
    Serial.println("No MQ3")                      //Check this code as not sure if correct.
    Serial.println("No MQ6")
    Serial.println("No MQ7")
    Serial.println("No MQ8")
    Serial.println("No MQ135")
    while(1);
  } {                                              //Check this out, may need to define them above separetely.
  delay(2000)                                     //Intialise sensors.
}
  Serial.print("Calibrating...\n");                
  Ro = MQCalibration(MQ3_PIN);                    //Calibrating the sensor. Enure the sensor is in clean air when you perform the calibration.
  Ro = MQCalibration(MQ6_PIN);
  Ro = MQCalibration(MQ7_PIN);
  Ro = MQCalibration(MQ8_PIN); 
  Ro = MQCalibration(MQ135_PIN);                   
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");
}
 
void loop()
{
  DateTime now = RTC.now();
  utc = (now.unixtime());
  sprintf(tmeStrng, "%04d/%02d/%02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second())
   Serial.print("TRC utc Time: ")
   Serial.print(now.unixtime());
   Serial.println();
   Serial.print("RTC time: ");
   Serial.print(tmeStrng);
   Serial.print("MQ3 Sensor:");
   Serial.print("C2H5OH:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ3_PIN)/Ro,GAS_C2H5OH) );
   Serial.print( "ppm" );
   Serial.print("    ");
   Serial.print("MQ5 Sensor:");   
   Serial.print("CH4:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ6_PIN)/Ro,GAS_CH4) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("MQ7 Sensor:");   
   Serial.print("CO:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ7_PIN)/Ro,GAS_CO) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("MQ8 Sensor:");   
   Serial.print("H2:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ8_PIN)/Ro,GAS_H2) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("\n");
   Serial.print("MQ135 Sensor:");
   Serial.print("NH3:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ135_PIN)/Ro,GAS_NH3) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("CO2:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ135_PIN)/Ro,GAS_CO2) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("SECC2H5OH:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ135_PIN)/Ro,GAS_SECC2H5OH) );
   Serial.print( "ppm" );
   Serial.print("\n");
   Serial.print("C6H6:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ135_PIN)/Ro,GAS_C6H6) );
   Serial.print( "ppm" );
   Serial.print("\n");
   Serial.print();
   delay(1000);
}
 
/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/ 
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
 
/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
************************************************************************************/ 
float MQCalibration(int mq_pin)

{
  int i;
  float val=0;
 
  for (i=0;i<CALIBRATION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBRATION_SAMPLE_TIMES;                   //calculate the average value
 
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 
 
  return val; 
}
/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
 
/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function passes different curves to the MQGetPercentage function which 
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/ 
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
   if ( gas_id == GAS_C2H5OH ) {
     return MQGetPercentage(rs_ro_ratio,C2H5OHCurve);
  } else if ( gas_id == GAS_CH4 ) {
     return MQGetPercentage(rs_ro_ratio,CH4Curve);
  } else if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_H2 ) {
     return MQGetPercentage(rs_ro_ratio,H2Curve);
  } else if ( gas_id == GAS_NH3 ) {
     return MQGetPercentage(rs_ro_ratio,NH3Curve);
  } else if ( gas_id == GAS_CO2 ) {
     return MQGetPercentage(rs_ro_ratio,CO2Curve);
  } else if ( gas_id == GAS_SECC2H5OH ) {
     return MQGetPercentage(rs_ro_ratio,SECC2H5OHCurve);  
  } else if ( gas_id == GAS_C6H6 ) {
     return MQGetPercentage(rs_ro_ratio,C6H6Curve);  
  }
  return 0;
}
 
/*****************************  MQGetPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
//////////////////////////////////////////////Write data to SD card//////////////////
File dataFile = SD.open("datalog.txt", FILE_WRITE);          //To write the data to the SD card.
dataFile.print(tmeStrng);dataFile.print(",");
dataFile.print(utc);dataFile.print(",");
dataFile.print(MQ3);dataFile.print("C2H5OH")
dataFile.print(MQ6);dataFile.print("CH4")
dataFile.print(MQ7);dataFile.print("CO")
dataFile.print(MQ8);dataFile.print("H2")
dataFile.print(MQ135);dataFile.print("NH3")
dataFile.print(MQ135);dataFile.print("CO2")
dataFile.print(MQ135);dataFile.print("SECC2H5OH")
dataFile.print(MQ135);dataFile.print("C6H6")
dataFile.close();
delay(logMillis);
}

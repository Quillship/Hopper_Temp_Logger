#include "SD.h"                    
#include <Wire.h>                 
#include <RTClib.h>
#include <OneWire.h>            
#include <DallasTemperature.h>


// Initalize Variables

#define LEDpin 4

#define ONE_WIRE_BUS 8

float intempC = 0;
float outtempC = 0;

const int chipSelect = 10;
                         
                         
File datalog;
char filename[] = "LOGGER00.csv";
RTC_DS1307 RTC;


  // Initalize Digital temperature sensor

  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
     OneWire oneWire(ONE_WIRE_BUS);

  // Pass our oneWire reference to Dallas Temperature. 
     DallasTemperature sensors(&oneWire);

  // Arrays to hold device address
     DeviceAddress insideThermometer;
     DeviceAddress outsideThermometer;

void setup()
{
  Serial.begin(9600); // Opens the communication between Arduino and computer serial port
  
  Wire.begin();         // Begins RTC communication
  RTC.begin();
  sensors.begin();      // Begins digital temp sensor
  
// Set RTC
  RTC.adjust(DateTime(__DATE__, __TIME__));
 
// Initalize SD Card  
  Serial.print("Intializing SD Card...");
  pinMode(10, OUTPUT); // Necessary for microSD shield
  
  if(!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized.");  
  
  Serial.print("Creating File...");
  

// Make a new file each time the arduino is powered

  for (uint8_t i = 0; i < 100; i++) 
  {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) 
    {
      // only open a new file if it doesn't exist
      datalog = SD.open(filename, FILE_WRITE); 
      break;
    }
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);
  
  if(!datalog)
  {
  Serial.println("Couldn't Create File"); 
  return;
  }
  
// Print Header
  String Header = "RTC Date, RTC Time, In Temp [F], Out Temp [F]";
  
  datalog = SD.open(filename, FILE_WRITE);
  datalog.println(Header);
  Serial.println(Header); 
   
// Get address of digital temp sensor 

  if (!sensors.getAddress(insideThermometer, 0)) {
    datalog.println("Unable to find address for Device 0");
    Serial.println("Unable to find address for Device 0");
  }

 // Set the resolution to 12 bit 
  sensors.setResolution(insideThermometer, 12);    
  sensors.setResolution(outsideThermometer, 12);
     
  datalog.close();
  
  digitalWrite(LEDpin, OUTPUT);
}

//___________________________________________________________________________________

void loop()
{  
datalog = SD.open(filename, FILE_WRITE);
 
// Real Time Clock

  DateTime now = RTC.now();

  datalog.print(now.month(), DEC);
  datalog.print("/");
  datalog.print(now.day(), DEC);
  datalog.print("/");
  datalog.print(now.year(), DEC);
  datalog.print(",");
  datalog.print(now.hour(), DEC);
  datalog.print(":");
  datalog.print(now.minute(), DEC);
  datalog.print(":");
  datalog.print(now.second(), DEC);
  datalog.print(",");
 
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.year(), DEC);
  Serial.print(", ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(", ");
  
  
// Digital Temperature

  sensors.requestTemperatures(); // There might be a better place for this function

  intempC = sensors.getTempC(insideThermometer);
  outtempC = sensors.getTempC(outsideThermometer);

  intempC = (intempC*1.8)+32;
  outtempC = (outtempC*1.8)+32;

  datalog.print(intempC);
  datalog.print(",");
  datalog.print(outtempC);

  Serial.print(intempC);
  Serial.print(", ");
  Serial.print(outtempC);
  


// Close the file and flash LED to show data was recorded

datalog.println("");
Serial.println("");

datalog.close(); 

digitalWrite(LEDpin, HIGH);  //Flash a LED to know that it is logging data
//Time between readings
delay(60000);
digitalWrite(LEDpin, LOW);
}

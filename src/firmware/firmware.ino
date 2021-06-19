/*
 * This project is based on the project available at https://www.diymachines.co.uk/how-to-build-a-giant-hidden-shelf-edge-clock
 *
 * Buy me a coffee to say thanks: https://ko-fi.com/marcoaureliorb
 * 
 * 
 * Bluetooh module
 *    pin D2 - TX
 *    pin D3 - RX
 * Light sensor
 *    pin A0
 * Temperature sensor
 *    pin D7
 * Clock module
 *    pin A4 - SDA
 *    pin A5 - SCL
 * Light clock
 *    pin D5
 * Light down   
 *    pin D4
*/

#include <Adafruit_NeoPixel.h>
#include "Wire.h"
#include "DHT.h"

#define DHTPIN 7      
#define DHTTYPE DHT11 

#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE
#include <Dabble.h>

#define PCF8563address 0x51

#define LEDCLOCK_PIN    5
#define LEDCLOCK_COUNT 252

#define LEDDOWNLIGHT_PIN    4
#define LEDDOWNLIGHT_COUNT 14

#define NUM_LED_SEGMENT 9

/*
   28 parts
   09 leds per part / 252 leds
   3A 
  
  *********   ********   *********   ********   *********   ********   ********* 
 *    22   * *        * *    15   * *        * *    8    * *        * *    1    *
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 * 23   21 * *        * * 16   14 * *        * * 9     7 * *        * * 2     0 * 
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 *    24   * *        * *    17   * *        * *    10   * *        * *    3    *
  *********   ********   *********   ********   *********   ********   ********* 
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 * 27   25 * *        * * 20   18 * *        * * 13   11 * *        * * 6     4 *
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 *         * *        * *         * *        * *         * *        * *         *
 *   26    * *        * *    19   * *        * *    12   * *        * *    5    *
  *********   ********   ********    ********   *********   ********   ********* 
*/

// Initial config for date and time.
int second     = 0;
int minute     = 0;
int hour       = 12;
int dayOfWeek  = 5; //friday
int dayOfMonth = 1;
int month      = 1;
int year       = 2021;

String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

int clockMinutecolor = 0;
int clockHourcolor   = 0;
int clockFaceBrightness = 0;

int downLightercolor = 0;
bool downLighterStatus = true;

int temperature = 22;
int tempValuecolor  = 0;
int tempSymbolcolor = 0;

unsigned long millisAtualizacao = millis();

static String command = "\0";

DHT dht(DHTPIN, DHTTYPE);

Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);

//Smoothing of the readings from the light sensor so it is not too twitchy
const int numReadings = 12;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0;         // the index of the current reading
long total = 0;            // the running total
long average = 0;          // the average

void setup() {
  
  Serial.begin(9600);
  Dabble.begin(9600);
  Wire.begin();
  dht.begin();
  
  clockMinutecolor = colorToInt(0, 0, 255);
  clockHourcolor   = colorToInt(255, 255, 255);
  downLightercolor = colorToInt(255, 255, 255);
  tempValuecolor   = colorToInt(0, 0, 255);
  tempSymbolcolor  = colorToInt(255, 255, 255);
  
  clockFaceBrightness = 100;  
   
  stripClock.begin();
  stripClock.show();
  stripClock.setBrightness(clockFaceBrightness); //Set inital BRIGHTNESS (max = 255)

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  setClockBrightness();
  setDownLighter();
  
}

void loop() {
  
  readTheBluetoothCommand();
  
  if((millis() - millisAtualizacao) <= 45000){
    readTheLightSensor();
    readTheTime();
    displayTheTime();
  }
  else{
    readTheTemperature();
    displayTheTemperature();
  }

  if((millis() - millisAtualizacao) >=  60000){
    millisAtualizacao = millis();
  }  
  
}

void setDownLighter(){

  //Serial.print("Down Lighter status = ");
  //Serial.println(downLighterStatus);

  //Serial.print("Down Lighter color = ");
  //Serial.println(downLightercolor);
    
  if(downLighterStatus){  
    stripDownlighter.fill(downLightercolor, 0, LEDDOWNLIGHT_COUNT);
    stripDownlighter.show();
  }
  else{
    stripDownlighter.clear();
  }
  
}

void setClockBrightness(){
  
  //Serial.print("ClockBrightness: ");
  //Serial.println(clockFaceBrightness);
  stripClock.setBrightness(clockFaceBrightness);
  stripClock.show();
  
}

void readTheBluetoothCommand(){
  
  Dabble.processInput();  
 
  if(Terminal.available())
  {
    char caracter = char(Terminal.read());

    if(caracter != '#'){
      command += caracter;
      Serial.print("Comando: ");
      Serial.println(command);      
    }
    else{
      Serial.print("Comando: ");
      Serial.println(command);

      if (command.startsWith("CM")){//Command: CM255255255
        clockMinutecolor = colorToInt(command.substring(2));
      }
      else if (command.startsWith("CH")){//Command: CH255255255
        clockHourcolor = colorToInt(command.substring(2));
      }
      else if (command.startsWith("TV")){//Command: TV255255255
        tempValuecolor = colorToInt(command.substring(2));
      } 
      else if (command.startsWith("TS")){//Command: TS255255255
        tempSymbolcolor = colorToInt(command.substring(2));
      }
      else if (command.startsWith("DLS")){//Command: DLSON / DLSOFF
        downLighterStatus = command.substring(3,5) == "ON";
      } 
      else if (command.startsWith("DLC")){//Command: DLC255255255
        downLightercolor = colorToInt(command.substring(2));
      }   
      else if (command.startsWith("ST")){ //Command: STDOWDDMMYYHHMM
        
        String pDayOfWeek = String(command[2]);
        String pDayOfMonth = String(command[3]) + String(command[4]);
        String pMonth = String(command[5]) + String(command[6]);
        String pYear = String(command[7]) + String(command[8]);
        String pHour = String(command[9]) + String(command[10]);
        String pMinute = String(command[11]) + String(command[12]);
        int pSecond = 0;

        #ifdef DEBUG
        Serial.print(days[pDayOfWeek.toInt()]); 
        Serial.print(",");
        Serial.print(pDayOfMonth);
        Serial.print("/");
        Serial.print(pMonth);
        Serial.print("/");
        Serial.print(pYear);        
        Serial.print(" ");
        Serial.print(pHour);
        Serial.print(":");
        Serial.println(pMinute);
        #endif        
        
        setPCF8563(pHour.toInt(), pMinute.toInt(), pSecond, pDayOfWeek.toInt(), pDayOfMonth.toInt(), pMonth.toInt(), pYear.toInt());
      } 
   
      command = "\0";
      Serial.print("Comando: ");
      Serial.println(command);
    }
  }
  
}

void readTheLightSensor(){

  //Record a reading from the light sensor and add it to the array
  readings[readIndex] = analogRead(A0); //get an average light level from previouse set of samples
  
  ////Serial.print("Light sensor value added to array = ");
  ////Serial.println(readings[readIndex]);
  readIndex = readIndex + 1; // advance to the next position in the array:

  // if we're at the end of the array move the index back around...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  //now work out the sum of all the values in the array
  int sumBrightness = 0;
  for (int i=0; i < numReadings; i++)
  {
    sumBrightness += readings[i];
  }
  
  ////Serial.print("Sum of the brightness array = ");
  ////Serial.println(sumBrightness);

  // and calculate the average: 
  int lightSensorValue = sumBrightness / numReadings;
  ////Serial.print("Average light sensor value = ");
  ////Serial.println(lightSensorValue);

  //set the brightness based on ambiant light levels
  clockFaceBrightness = constrain(map(lightSensorValue,0, 1023, 200, 50), 50, 200); 
  
  //Serial.print("Mapped brightness value = ");
  //Serial.println(clockFaceBrightness);  
  
}

void readTheTime(){

  readPCF8563();

  //Serial.print(days[dayOfWeek]); 
  //Serial.print(" ");  
  //Serial.print(dayOfMonth);
  //Serial.print("/");
  //Serial.print(month);
  //Serial.print("/20");
  //Serial.print(year);
  //Serial.print(" - ");
  //Serial.print(hour);
  //Serial.print(":");
  if (minute < 10)
  {
    //Serial.print("0");
  }
  //Serial.print(minute);
  //Serial.print(":");  
  if (second < 10)
  {
    //Serial.print("0");
  }  
  
  //Serial.println(second);
  
}

void readTheTemperature(){
  
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //Serial.print("Temperature: ");
  //Serial.print(t);
  //Serial.println("ºC ");
  
}

void displayTheTime(){

  stripClock.clear(); //clear the clock face 

  //Serial.print("Clock Minute color = ");
  //Serial.println(clockMinutecolor); 
  int firstMinuteDigit = minute % 10; //work out the value of the first digit and then display it
  displayNumber(firstMinuteDigit, 0, clockMinutecolor);

  int secondMinuteDigit = floor(minute / 10); //work out the value for the second digit and then display it
  displayNumber(secondMinuteDigit, 63, clockMinutecolor);  

  //Serial.print("Clock hour color = ");
  //Serial.println(clockHourcolor);   
  int firstHourDigit = hour % 10; //work out the value for the third digit and then display it
  displayNumber(firstHourDigit, 126, clockHourcolor);

  int secondHourDigit = floor(hour / 10); //work out the value for the fourth digit and then display it
  displayNumber(firstHourDigit, 189, clockHourcolor);
  
  stripClock.show();
  
}

void displayTheTemperature(){

  stripClock.clear();

  //Serial.print("Temperature symbol color = ");
  //Serial.println(tempSymbolcolor); 
  letterC(0, tempSymbolcolor);  
  
  symbolDegrees(63, tempSymbolcolor);

  //Serial.print("Temperature digit color = ");
  //Serial.println(tempValuecolor); 
  int firstTempDigit = temperature % 10; //work out the value for the third digit and then display it
  displayNumber(firstTempDigit, 126, tempValuecolor);

  int secondTempDigit = floor(temperature / 10); //work out the value for the fourth digit and then display it
  displayNumber(secondTempDigit, 189, tempValuecolor);
    
  stripClock.show();
  
}

//----------------------------------------------------
//           Funcoes para leitura da hora
//----------------------------------------------------
void readPCF8563(){

  // this gets the time and date from the PCF8563

  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 7);
  second     = bcdToDec(Wire.read() & B01111111); // remove VL error bit
  minute     = bcdToDec(Wire.read() & B01111111); // remove unwanted bits from MSB
  hour       = bcdToDec(Wire.read() & B00111111); 
  dayOfMonth = bcdToDec(Wire.read() & B00111111);
  dayOfWeek  = bcdToDec(Wire.read() & B00000111);  
  month      = bcdToDec(Wire.read() & B00011111);  // remove century bit, 1999 is over
  year       = bcdToDec(Wire.read());
  
}

void setPCF8563(
  int pHour,
  int pMinute,  
  int pSecond,
  int pDayOfWeek,
  int pDayOfMonth,
  int pMonth,
  int pYear){

 // this sets the time and date to the PCF8563

  hour = pHour;
  minute = pMinute;
  second = pSecond;  
  
  //0 - SUN
  //1 - MON
  //2 - TUE
  //3 - WED
  //4 - THU
  //5 - FRI
  //6 - SAT
  dayOfWeek = pDayOfWeek;
  
  dayOfMonth = pDayOfMonth;
  month = pMonth;
  year = pYear;
  
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.write(decToBcd(second));  
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));     
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(dayOfWeek));  
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

/*
 * This project is based on the project available at https://www.diymachines.co.uk/how-to-build-a-giant-hidden-shelf-edge-clock
 *
 * Buy me a coffee to say thanks: https://ko-fi.com/marcoaureliorb
*/

#include <Adafruit_NeoPixel.h>
#include "Wire.h"
#include <Dabble.h>
#include "DHT.h"

#define DHTPIN 2      
#define DHTTYPE DHT11 

#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE

#define PCF8563address 0x51

#define LEDCLOCK_PIN    5
#define LEDCLOCK_COUNT 252

#define LEDDOWNLIGHT_PIN    3
#define LEDDOWNLIGHT_COUNT 14

#define NUM_LED_SEGMENT 9

#define QUINZE_SEGUNDOS 15 * 1000
#define QUARENTA_CINCO_SEGUNDOS 45 * 1000

/*
  28 partes
  09 leds por parte / 83.4 mA
  252 leds totais / 3A consumo
  
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

int second, minute, hour, dayOfWeek, dayOfMonth, month, year;
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

//(red * 65536) + (green * 256) + blue ->for 32-bit merged color value so 16777215 equals white
int clockMinutecolor = (  0 * 65536) + (  0 * 256) + 255;
int clockHourcolor   = (255 * 65536) + (255 * 256) + 255;
int clockFaceBrightness = 100;

int downLightercolor = (255 * 65536) + (255 * 256) + 255;
bool downLighterStatus = true;

int temperature = 22;
int tempValuecolor  = (  0 * 65536) + (  0 * 256) + 255;
int tempSymbolcolor = (255 * 65536) + (255 * 256) + 255;

DHT dht(DHTPIN, DHTTYPE);

// Declare our NeoPixel objects:
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);

//Smoothing of the readings from the light sensor so it is not too twitchy
const int numReadings = 12;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0;         // the index of the current reading
long total = 0;            // the running total
long average = 0;          // the average

String Serialdata = "";

void setup() {
  Serial.begin(9600);
  Dabble.begin(9600);
  Wire.begin();
  dht.begin();
  //setPCF8563(); Apenas para inicializar o relogio.
   
  stripClock.begin();
  stripClock.show();
  stripClock.setBrightness(clockFaceBrightness); //Set inital BRIGHTNESS (max = 255)

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  readTheTime();
  readTheLightSensor();
  readTheTemperature();
  readTheBluetoothCommand();
  
  setClockBrightness();
  setDownLighter();

  displayTheTime();  
  
  delay(QUARENTA_CINCO_SEGUNDOS);
  
  displayTheTemperature();
  
  delay(QUINZE_SEGUNDOS);

  delay(1000);
}

void setDownLighter(){
  
  Serial.print("Down Lighter status = ");
  Serial.println(downLighterStatus);

  Serial.print("Down Lighter color = ");
  Serial.println(downLightercolor);
    
  if(downLighterStatus){  
    stripDownlighter.fill(downLightercolor, 0, LEDDOWNLIGHT_COUNT);
    stripDownlighter.show();
  }
  else{
    stripDownlighter.clear();
  }
}

void setClockBrightness(){
  stripClock.setBrightness(clockFaceBrightness);
  stripClock.show();
}

void readTheBluetoothCommand(){
  
  bool dataflag = 0;
  
  Dabble.processInput();
  
  while (Serial.available() != 0)
  {
    Serialdata = String(Serialdata + char(Serial.read()));
    dataflag = 1;
  }
  
  if (dataflag == 1)
  {
    Serial.print(Serialdata);
  
  if (Serialdata.startsWith("CM")){//Command: CM 30FF45
    clockMinutecolor = hexToDec(Serialdata.substring(3));
  }
  else if (Serialdata.startsWith("CH")){//Command: CH 30FF45
    clockHourcolor = hexToDec(Serialdata.substring(3));
  }
  else if (Serialdata.startsWith("TV")){//Command: TV 30FF45
    tempValuecolor = hexToDec(Serialdata.substring(3));
  } 
  else if (Serialdata.startsWith("TS")){//Command: TS 30FF45
    tempSymbolcolor = hexToDec(Serialdata.substring(3));
  }
  else if (Serialdata.startsWith("DLS")){//Command: DLS ON
      downLighterStatus = Serialdata.substring(3) == "ON";
  } 
  else if (Serialdata.startsWith("DLC")){//Command: DLC 30FF45
      downLightercolor = hexToDec(Serialdata.substring(3));
  }   
  else if (Serialdata.startsWith("ST")){ //Command: ST DD/MM/YYYY HH:MM:SS:DayOfWeek
    
    Serialdata = Serialdata.substring(3);
    setPCF8563(Serialdata.substring(11, 13).toInt(), // Hora
               Serialdata.substring(14, 16).toInt(), // Minuto
               Serialdata.substring(17, 20).toInt(), // Segundo
               Serialdata.substring(20).toInt(),     // DayOfWeek
               Serialdata.substring(0,2).toInt(),    // DayOfMonth
               Serialdata.substring(3,5).toInt(),    // Month
               Serialdata.substring(6,10).toInt());  // Year
  } 
  
    Serialdata = "";
    dataflag = 0;
  }
  
  /*if(Terminal.available())
  {
    while (Terminal.available() != 0)
    {
      Serial.write(Terminal.read());
    }
    Serial.println();
  }
  */
}

void readTheLightSensor(){

  //Record a reading from the light sensor and add it to the array
  readings[readIndex] = analogRead(A0); //get an average light level from previouse set of samples
  
  Serial.print("Light sensor value added to array = ");
  Serial.println(readings[readIndex]);
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
  
  Serial.print("Sum of the brightness array = ");
  Serial.println(sumBrightness);

  // and calculate the average: 
  int lightSensorValue = sumBrightness / numReadings;
  Serial.print("Average light sensor value = ");
  Serial.println(lightSensorValue);

  //set the brightness based on ambiant light levels
  clockFaceBrightness = constrain(map(lightSensorValue,0, 1023, 200, 50), 50, 200); 
  
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);  
}


void readTheTime(){
  readPCF8563();

  Serial.print(days[dayOfWeek]); 
  Serial.print(" ");  
  Serial.print(dayOfMonth);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/20");
  Serial.print(year);
  Serial.print(" - ");
  Serial.print(hour);
  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute);
  Serial.print(":");  
  if (second < 10)
  {
    Serial.print("0");
  }  
  Serial.println(second);
}

void readTheTemperature(){
  
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print(F(" Humidity: "));
  //Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("C "));
  //Serial.print(f);
  //Serial.print(F("F  Heat index: "));
  //Serial.print(hic);
  //Serial.print(F("C "));
  //Serial.print(hif);
  //Serial.println(F("F"));
}

void displayTheTime(){

  stripClock.clear(); //clear the clock face 

  Serial.print("Clock Minute color = ");
  Serial.println(clockMinutecolor); 
  int firstMinuteDigit = minute % 10; //work out the value of the first digit and then display it
  displayNumber(firstMinuteDigit, 0, clockMinutecolor);

  int secondMinuteDigit = floor(minute / 10); //work out the value for the second digit and then display it
  displayNumber(secondMinuteDigit, 63, clockMinutecolor);  

  // -----------------------------------------------------

  Serial.print("Clock hour color = ");
  Serial.println(clockHourcolor);   
  int firstHourDigit = hour % 10; //work out the value for the third digit and then display it
  displayNumber(firstHourDigit, 126, clockHourcolor);

  int secondHourDigit = floor(hour / 10); //work out the value for the fourth digit and then display it
  displayNumber(firstHourDigit, 189, clockHourcolor);
  
  stripClock.show();
}

void displayTheTemperature(){

  stripClock.clear();

  letterC(0, tempSymbolcolor);  
  
  symbolDegrees(63, tempSymbolcolor);

  int firstTempDigit = temperature % 10; //work out the value for the third digit and then display it
  displayNumber(firstTempDigit, 126, tempValuecolor);

  int secondTempDigit = floor(temperature / 10); //work out the value for the fourth digit and then display it
  displayNumber(secondTempDigit, 189, tempValuecolor);
    
  stripClock.show();
}

//----------------------------------------------------
//           Funcoes para leitura da hora
//----------------------------------------------------
void readPCF8563()
// this gets the time and date from the PCF8563
{
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
  int pYear)
// this sets the time and date to the PCF8563
{
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
//----------------------------------------------------

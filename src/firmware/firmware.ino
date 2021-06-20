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

#define CLOCK_HOUR_COLOR         "CHC"
#define CLOCK_MINUTE_COLOR       "CMC"
#define TEMPERATURE_VALUE_COLOR  "TVC"
#define TEMPERATURE_SYMBOL_COLOR "TSC"
#define DOWN_LIGHT_STATE         "DLS"
#define DOWN_LIGTH_COLOR         "DLC"
#define DOWN_LIGHT_BRIGHTNESS    "DLB"
#define SET_DATE_TIME            "SDT"

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

uint32_t clockMinutecolor = 0;
uint32_t clockHourcolor   = 0;
int clockFaceBrightness = 0;

uint32_t downLightercolor = 0;
bool downLighterStatus = true;
int downLighterBrightness = 0;

int temperature = 0;
uint32_t tempValuecolor  = 0;
uint32_t tempSymbolcolor = 0;

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

  Serial.println("Setup start");
  
  Dabble.begin(9600);
  Wire.begin();
  dht.begin();

  clockFaceBrightness = 100; 
  downLighterBrightness = 50;
  
  stripClock.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();            // Turn OFF all pixels ASAP
  stripClock.setBrightness(clockFaceBrightness); // Set inital BRIGHTNESS (max = 255)

  stripDownlighter.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripDownlighter.show();            // Turn OFF all pixels ASAP
  stripDownlighter.setBrightness(downLighterBrightness); // Set BRIGHTNESS (max = 255)  
  
  clockMinutecolor = colorToInt(0, 0, 255);
  clockHourcolor   = colorToInt(255, 255, 255);
  downLightercolor = colorToInt(255, 255, 255);
  tempValuecolor   = colorToInt(0, 0, 255);
  tempSymbolcolor  = colorToInt(255, 255, 255);
   
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
  setDownLighter();
  
  Serial.println("Setup finish");
}

void loop() {
  readTheBluetoothCommand();
  
  if((millis() - millisAtualizacao) <= 45000){
    readTheLightSensor();
    readTheDateTime();
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
    stripDownlighter.show();
  }
}

void setClockBrightness(){
  Serial.print("ClockBrightness: ");
  Serial.println(clockFaceBrightness);
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
    }
    else{
        Serial.print("Comando: ");
        Serial.println(command);
        
        String parametros = command.substring(3);
        String resposta = "command failed";
  
        if (command.startsWith(CLOCK_MINUTE_COLOR)){
          resposta = handleClockMinuteColorChange(parametros);
        }
        else if (command.startsWith(CLOCK_HOUR_COLOR)){
          resposta = handleClockHourColorChange(parametros);
        }
        else if (command.startsWith(TEMPERATURE_VALUE_COLOR)){
          resposta = handleTemperatureValueColorChange(parametros);
        } 
        else if (command.startsWith(TEMPERATURE_SYMBOL_COLOR)){
          resposta = handleTemperatureSymbolColorChange(parametros);
        }
        else if (command.startsWith(DOWN_LIGHT_STATE)){
          resposta = handleDownLightStateChange(parametros);
        } 
        else if (command.startsWith(DOWN_LIGTH_COLOR)){
          resposta = handleDownLightColorChange(parametros);
        }   
        else if (command.startsWith(DOWN_LIGHT_BRIGHTNESS)){
          resposta = handleDownLightBrightnessChange(parametros);
        }
        else if (command.startsWith(SET_DATE_TIME)){
          resposta = handleSetDateTime(parametros);
        } 

      command = "\0";
      Terminal.println(resposta);
    }
  }
}

void readTheLightSensor(){

  //Record a reading from the light sensor and add it to the array
  readings[readIndex] = analogRead(A0); //get an average light level from previouse set of samples

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
  
  // and calculate the average: 
  int lightSensorValue = sumBrightness / numReadings;

  //set the brightness based on ambiant light levels
  clockFaceBrightness = constrain(map(lightSensorValue,0, 1023, 200, 50), 50, 200); 
  
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);  
}

void readTheTemperature(){
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("ºC ");  
}

void displayTheTime(){

  stripClock.clear(); //clear the clock face

  stripClock.setBrightness(clockFaceBrightness); //Set inital BRIGHTNESS (max = 255)    

  int firstMinuteDigit = minute % 10; //work out the value of the first digit and then display it
  displayNumber(firstMinuteDigit, 0, clockMinutecolor);

  int secondMinuteDigit = floor(minute / 10); //work out the value for the second digit and then display it
  displayNumber(secondMinuteDigit, 63, clockMinutecolor);  

  int firstHourDigit = hour % 10; //work out the value for the third digit and then display it
  displayNumber(firstHourDigit, 126, clockHourcolor);

  int secondHourDigit = floor(hour / 10); //work out the value for the fourth digit and then display it
  displayNumber(firstHourDigit, 189, clockHourcolor);
  
  stripClock.show();  
}

void displayTheTemperature(){
  stripClock.clear();

  displayLetterC(0, tempSymbolcolor);  
  displaySymbolDegrees(63, tempSymbolcolor);

  int firstTempDigit = temperature % 10; //work out the value for the third digit and then display it
  displayNumber(firstTempDigit, 126, tempValuecolor);

  int secondTempDigit = floor(temperature / 10); //work out the value for the fourth digit and then display it
  displayNumber(secondTempDigit, 189, tempValuecolor);
    
  stripClock.show();  
}

//----------------------------------------------------
//           API Bluetooth function
//----------------------------------------------------

String handleClockHourColorChange(String parametros){
  clockHourcolor = colorToInt(parametros);
  
  return "Clock Hour Color changed";
}

String handleClockMinuteColorChange(String parametros){
  clockMinutecolor = colorToInt(parametros);
  
  return "Clock Minute Color changed";
}

String handleTemperatureValueColorChange(String parametros){
  tempValuecolor = colorToInt(parametros);
  
  return "Temperature Value Color changed";  
}

String handleTemperatureSymbolColorChange(String parametros){
  tempSymbolcolor = colorToInt(parametros);
  
  return "Temperature Symbol Color changed";
}

String handleDownLightStateChange(String parametro){
  downLighterStatus = parametro == "ON";
  setDownLighter();
  
  return "Down Lighter Status changed";  
}

String handleDownLightColorChange(String parametros){
  downLightercolor = colorToInt(parametros);
  downLighterStatus = true;
  setDownLighter();
  
  return "Down Lighter Color changed";
}

String handleDownLightBrightnessChange(String parametro){
  downLighterBrightness = parametro.toInt();
  downLighterStatus = true;        
  setDownLighter();
  
  return "Down Lighter Brightness changed";   
}

String handleSetDateTime(String parametros){
  
  String pDayOfWeek = String(command[0]);
  String pDayOfMonth = String(command[1]) + String(command[2]);
  String pMonth = String(command[3]) + String(command[4]);
  String pYear = "20" + String(command[5]) + String(command[6]);
  String pHour = String(command[7]) + String(command[8]);
  String pMinute = String(command[9]) + String(command[10]);
  int pSecond = 0;
  writeTheDateTime(pHour.toInt(), pMinute.toInt(), pSecond, pDayOfWeek.toInt(), pDayOfMonth.toInt(), pMonth.toInt(), pYear.toInt());  
  
  return "Date and Time changed";
}

//----------------------------------------------------
//           Funcoes para leitura da hora
//----------------------------------------------------
void readTheDateTime(){
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

  printDateTime("DateTime read", hour, minute, second, days[dayOfWeek], dayOfMonth, month, year);
}

void writeTheDateTime(
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

  printDateTime("DateTime wirte", hour, minute, second, days[dayOfWeek], dayOfMonth, month, year);
}

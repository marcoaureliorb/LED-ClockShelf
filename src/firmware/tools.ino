void printDateTime(
  String reader,
  int pHour,
  int pMinute,  
  int pSecond,
  String pDayOfWeek,
  int pDayOfMonth,
  int pMonth,
  int pYear){
    Serial.print(reader);
    Serial.print(" ");
    Serial.print(pDayOfWeek); 
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
}

uint32_t colorToInt(String color){
  String red   = String(color[0]) + String(color[1]) + String(color[2]);
  String green = String(color[3]) + String(color[4]) + String(color[5]);
  String blue  = String(color[6]) + String(color[7]) + String(color[8]);
  
  return colorToInt(red.toInt(), green.toInt(), blue.toInt());
}

uint32_t colorToInt(int red, int green, int blue){
  // (red * 65536) + (green * 256) + blue ->for 32-bit merged color value so 16777215 equals white
  return ((uint32_t)red << 16) | ((uint32_t)green <<  8) | blue;
}

byte bcdToDec(byte value)
{
  return ((value / 16) * 10 + value % 16);
}

byte decToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}

long hexToDec(String hexString) 
{
  long decValue = 0;
  int nextInt;
  
  for (int i = 2; i < hexString.length(); i++) {

    if (i==4) i= 0;
    if (i==2 && decValue > 10) i= 4;
    
    nextInt = int(hexString.charAt(i));
  
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    
  nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  
  return decValue;
}

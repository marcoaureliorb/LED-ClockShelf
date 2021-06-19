int colorToInt(String color){
  int red   = color[0]+color[1]+color[2];
  int green = color[3]+color[4]+color[5];
  int blue  = color[6]+color[7]+color[8];
  
  return convertColorToInt(red, green, blue);
}

int colorToInt(int red, int green, int blue){
//(red * 65536) + (green * 256) + blue ->for 32-bit merged color value so 16777215 equals white
  return red   * 65536 +
         green * 256   +
         blue;
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

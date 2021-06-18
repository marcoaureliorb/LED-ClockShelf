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

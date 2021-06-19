void displayNumber(int digitToDisplay, int offsetBy, int color){
    switch (digitToDisplay){
    case 0:
    digitZero(offsetBy,color);
      break;
    case 1:
      digitOne(offsetBy,color);
      break;
    case 2:
    digitTwo(offsetBy,color);
      break;
    case 3:
    digitThree(offsetBy,color);
      break;
    case 4:
    digitFour(offsetBy,color);
      break;
    case 5:
    digitFive(offsetBy,color);
      break;
    case 6:
    digitSix(offsetBy,color);
      break;
    case 7:
    digitSeven(offsetBy,color);
      break;
    case 8:
    digitEight(offsetBy,color);
      break;
    case 9:
    digitNine(offsetBy,color);
      break;
    default:
     break;
  }
}

void digitZero(int offset, uint32_t color){
    segmentA(offset, color);
    segmentB(offset, color);
    segmentC(offset, color);
    segmentE(offset, color);
    segmentF(offset, color);
    segmentG(offset, color);  
}

void digitOne(int offset, uint32_t color){
    segmentA(offset, color);
    segmentE(offset, color);
}

void digitTwo(int offset, uint32_t color){
    segmentA(offset, color);
    segmentB(offset, color);
    segmentD(offset, color);
    segmentF(offset, color);
    segmentG(offset, color);
}

void digitThree(int offset, uint32_t color){
    segmentA(offset, color);
    segmentB(offset, color);
    segmentD(offset, color);
    segmentE(offset, color);
    segmentF(offset, color);
}

void digitFour(int offset, uint32_t color){
    segmentA(offset, color);
    segmentC(offset, color);
    segmentD(offset, color);
    segmentE(offset, color);
}

void digitFive(int offset, uint32_t color){
    segmentB(offset, color);
    segmentC(offset, color);
    segmentD(offset, color);
    segmentE(offset, color);
    segmentF(offset, color);
}

void digitSix(int offset, uint32_t color){
  segmentC(offset, color);
  segmentD(offset, color);
  segmentE(offset, color);
  segmentF(offset, color);
  segmentG(offset, color);
}

void digitSeven(int offset, uint32_t color){
    segmentA(offset, color);
  segmentB(offset, color);
  segmentE(offset, color);
}

void digitEight(int offset, uint32_t color){
    segmentA(offset, color);
  segmentB(offset, color);
  segmentC(offset, color);
  segmentD(offset, color);
  segmentE(offset, color);
  segmentF(offset, color);
  segmentG(offset, color);
}

void digitNine(int offset, uint32_t color){
  segmentA(offset, color);
  segmentB(offset, color);
  segmentC(offset, color);
  segmentD(offset, color);
  segmentE(offset, color);
}

void letterC(int offset, int color){
  segmentB(offset, color);
  segmentC(offset, color);
  segmentF(offset, color);
  segmentG(offset, color);
}

void symbolDegrees(int offset, int color){
  segmentA(offset, color);
  segmentB(offset, color);
  segmentC(offset, color);
  segmentD(offset, color);
}

/*
 * Segments locations
 
                  *********  
                 *    B    * 
                 *         * 
                 *         * 
                 * C     A * 
                 *         * 
                 *         * 
                 *         * 
                 *    D    * 
                  *********  
                 *         * 
                 *         * 
                 *         * 
                 * G     E * 
                 *         * 
                 *         * 
                 *         * 
                 *    F    * 
                  *********  
*/
void segmentA(int offset, int color){
  stripClock.fill(color, 0 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}                                              

void segmentB(int offset, int color){                               
  stripClock.fill(color, 1 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}                                              

void segmentC(int offset, int color){                               
  stripClock.fill(color, 2 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}                                              

void segmentD(int offset, int color){                               
  stripClock.fill(color, 3 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}                                              

void segmentE(int offset, int color){                               
  stripClock.fill(color, 4 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}                                              

void segmentF(int offset, int color){                               
  stripClock.fill(color, 5 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}
                                              
void segmentG(int offset, int color){                               
  stripClock.fill(color, 6 * NUM_LED_SEGMENT + offset, NUM_LED_SEGMENT);
}

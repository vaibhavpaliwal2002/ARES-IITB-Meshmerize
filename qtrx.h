#define d 7 // distance between sensors in mm
#define L -24.5 // left most sensor distance from the centre in mm (-ve)
int sensMax[] = {0,0,0,0,0,0,0,0}; // init values // will be updated from calibration function
int sensMin[] = {255,255,255,255,255,255,255,255};
int sensorRaw[8];
int sensVal[8]; 
int PCX[] = {PC3,PC2,PC1,PC0};//A0,A1,A2,A3 portc connections; note PCX[i] -> Ai
int PDX[] = {PD2,PD3,PD4,PD7};//D2,D3,D4,D7
int j,i=0;
float linePos = 0,lastPos;
void readSensor(){
//PORTC shift to TIMER1
i=0;
while(i<4){
  // charge capacitor  
  TCCR2B = (TCCR2B&B11111000)|0x1; // set prescaler to 1.. generate 16 Mhz clock  
  DDRC |= (1<<PCX[i]); // set A0 as o/p  
  TCNT2 = 0x0; // reset counter
  PORTC |= 1<<PCX[i]; // write high to PCi/Ai
  while(TCNT2<160); // wait for  10 us.. Tclk = 1/16M sec = 0.0625 usec .. Tdelay = 160*0.0625 usec = 10usec
  // charge capacitor  

  // set to High Z state
  DDRC &= ~(1<<PCX[i]); // set Ai as i/p (High Z state)
  PORTC &= ~(1<<PCX[i]); // write low to Ai / disable pull up now that the pin is set as input
  // set to High Z state 

  TCCR2B = (TCCR2B&B11111000)|0x5; // set prescaler to 128.. generate 16/128 Mhz clock .. for a resolution of 128/16 us and max time duration of 128*256/16 = 2.048ms 

  // OVF optimization
  TCNT2 = 0x0;
  while(TCNT2<20); // minimum delay is 28 clock cycles hence, start counting after 20 clock cycles taking in account an 8 cc buffer
  // OVF optimization

  // fall time calculation
  TCNT2 = 0x0; // reset and start counting
  while((PINC>>PCX[i])&0x1 && TCNT2 != 255); // while pin is detected high
  sensorRaw[i] = TCNT2; //note fall time on low detect
  // fall time calculation
  i++;
}

  // PORT D
  i=0;
  while(i<4){
  // charge capacitor  
  TCCR2B = (TCCR2B&B11111000)|0x1; // set prescaler to 1.. generate 16 Mhz clock  
  DDRD |= (1<<PDX[i]); // set A0 as o/p  
  TCNT2 = 0x0; // reset counter
  PORTD |= 1<<PDX[i]; // write high to PC0/A0
  while(TCNT2<160); // wait for  10 us.. Tclk = 1/16M sec = 0.0625 usec .. Tdelay = 160*0.0625 usec = 10usec
  // charge capacitor  

  // set to High Z state
  DDRD &= ~(1<<PDX[i]); // set A0 as i/p (High Z state)
  PORTD &= ~(1<<PDX[i]); // write low to A0 / disable pull up now that the pin is set as input
  // set to High Z state 

  TCCR2B = (TCCR2B&B11111000)|0x5; // set prescaler to 256.. generate 16/256 Mhz clock .. for a resolution of 256/16 us and max time duration of 256*256/16 = 4.096ms 

  // OVF optimization
  TCNT2 = 0x0;
  while(TCNT2<20); // minimum delay is 28 clock cycles hence, start counting after 20 clock cycles taking in account a 8 cc buffer
  // OVF optimization

  // fall time calculation
  TCNT2 = 0x0; // reset and start counting
  while((PIND>>PDX[i])&0x1 && TCNT2 != 255); // while pin is detected high and TCNT2 still has counting space left  
  sensorRaw[i+4] = TCNT2; //note fall time on low detect
  // fall time calculation
  i++;
}

// normalise the values between 0 and 255 
lastPos = linePos;
linePos = 0; // clear
for (i=0;i<8;i++)
{
  sensVal[i] = (255.0*(sensorRaw[i]-sensMin[i]))/(sensMax[i]-sensMin[i]); // map sensor values between 0 and 255 
  sensVal[i] = sensVal[i]>255?255:sensVal[i]<0?0:sensVal[i]; 
  linePos+= (L+i*d)*sensVal[i]; // L is the posn of the left most sensor and d is the spacing b/w sensors
}
linePos/=1.0*sensVal[0]+sensVal[1]+sensVal[2]+sensVal[3]+sensVal[4]+sensVal[5]+sensVal[6]+sensVal[7]; // normalize

}
// run once on startup
void calibration(){
  // slowly turn left and capture max and min sensor values
  int k;
  while(millis()< 6000){ // calib time .. net turning time
    // ******turn left
    if(millis()<4000); // turn right
    else; // turn right
    readSensor();
    for (k=0;k<8;k++) // update min and max
      if(sensMax[k]<sensorRaw[k])sensMax[k]=sensorRaw[k];
      else if(sensMin[k]>sensorRaw[k])sensMin[k]=sensorRaw[k];
  }
  
}

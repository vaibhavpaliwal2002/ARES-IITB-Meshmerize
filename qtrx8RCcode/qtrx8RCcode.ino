// Device : Pololu QTRX 8RC line sensor
unsigned short int sensorRaw[8] ;

int PCX[] = {PC0,PC1,PC2,PC3};//A0,A1,A2,A3 portc connections; note PCX[i] -> Ai
int PDX[] = {PD2,PD3,PD4,PD7};//D2,D3,D4,D7
int i=0;
void readSensor(){
//PORTC
i=0;
while(i<4){
// charge capacitor  
TCCR2B = (TCCR2B&B11111000)|0x1; // set prescaler to 1.. generate 16 Mhz clock  
DDRC |= (1<<PCX[i]); // set A0 as o/p  
TCNT2 = 0x0; // reset counter
PORTC |= 1<<PCX[i]; // write high to PC0/A0
while(TCNT2<180); // wait for  10 us.. Tclk = 1/16M sec = 0.0625 usec .. Tdelay = 160*0.0625 usec = 10usec
// charge capacitor  

// set to High Z state
DDRC &= ~(1<<PCX[i]); // set A0 as i/p (High Z state)
PORTC &= ~(1<<PCX[i]); // write low to A0 / disable pull up now that the pin is set as input
// set to High Z state 

TCCR2B = (TCCR2B&B11111000)|0x5; // set prescaler to 256.. generate 16/256 Mhz clock .. for a resolution of 256/16 us and max time duration of 256*256/16 = 4.096ms 

// OVF optimization
TCNT2 = 0x0;
while(TCNT2<20); // minimum delay is 28 clock cycles hence, start counting after 20 clock cycles taking in account a 8 cc buffer
// OVF optimization

// fall time calculation
TCNT2 = 0x0; // reset and start counting
while((PINC>>PCX[i])&0x1); // while pin is detected high
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
while(TCNT2<180); // wait for  10 us.. Tclk = 1/16M sec = 0.0625 usec .. Tdelay = 160*0.0625 usec = 10usec
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
while((PIND>>PDX[i])&0x1); // while pin is detected high
sensorRaw[i+4] = TCNT2; //note fall time on low detect
// fall time calculation
i++;
}

//NOTES:
// any time greater than 2.048 ms will lead to overflow! change to counter 1 for greater range if required   
// add overlow exception using T2OVF if required .

}


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
TCCR2A = 0x0; // disable port2 for PWM and CTC
}

void loop() {
  // put your main code here, to run repeatedly:
readSensor();
delay(100);
for(int j =0;j<8;j++){
Serial.print(sensorRaw[j]);
Serial.print("\t");
}
Serial.println();
}

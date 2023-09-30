

void setup() {
  // put your setup code here, to run once:
  DDRB |= (1 << 1);

  PORTB |= (1 << 1);
  delay(250);
  PORTB &= ~(1 << 1);
  delay(250);
  PORTB |= (1 << 1);
  delay(250);
  PORTB &= ~(1 << 1);
  delay(250);
  PORTB |= (1 << 1);
  delay(250);
  PORTB &= ~(1 << 1);
  delay(250);

  TCCR1A = (1 << COM1A0);                   // CTC MODE 
  OCR1A = 13;                               //CMNT IT OUT LATER!
  TCNT1 = 20;                               // MAX NUMBER IT SHOULD COUNT THAT GENERATES 800KHz
  TCCR1B = (1 << CS00) | (1 << WGM12);      //RUN THE TIMER0 WITH 16MHz NO DIVISION
  //delay(500);
}

#define BIT1_H  13
#define BIT0_H  7  

void Ws2812_bitGenerator( uint32_t data ){
 for(uint32_t counter = 0; counter < 24; ++counter){
   while((TIFR0 & (1 << TOV0)) != (1 << TOV0));
   TIFR0 |= (1 << TOV0);   
   if((data & (1 << 23)) == 1){
     OCR0A = BIT1_H;
   }
   else{
     OCR0A = BIT0_H;
   }
   data = (data << 1);
  }
  while((TIFR0 & (1 << TOV0)) != (1 << TOV0));
  TIFR0 |= (1 << TOV0);   
  OCR0A = 0;
}

uint32_t ledNum[64] = {
  0X00AABBCC
};

void loop() {
  // put your main code here, to run repeatedly:
  //delayMicroseconds(100);
  //Ws2812_bitGenerator(ledNum[0]);
  //while((TIFR1 & (1 << OCF1A)) != (1 << OCF1A));
  //TIFR1 |= (1 << OCF1A); 
  //if((TIFR1 & (1 << TOV1)) == (1 << TOV1)){
  //  TIFR1 = (1 << TOV1);   
    //PORTB |= (1 << 1);
   // delay(250);
   // PORTB &= ~(1 << 1);
   // delay(250);
    //OCR1A = 1000;                              //CMNT IT OUT LATER!
    //TCNT1 = 2000;
  //}

}

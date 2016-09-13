//timer interrupts
//by Amanda Ghassaei
//June 2012
//http://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable all three arduino timer interrupts.
//timer0 will interrupt at 2kHz
//timer1 will interrupt at 1Hz
//timer2 will interrupt at 8kHz

//storage variables
volatile boolean toggle0 = 1;
volatile boolean toggle1 = 0;
volatile boolean toggle2 = 0;

volatile uint32_t currTime = 0; 


uint32_t deltat = 0, sum = 0, time_sec = 0, time_sec2 = 0;          // integration interval for both filter schemes
uint32_t lastUpdate = 0, firstUpdate = 0; // used to calculate integration interval
uint32_t Now = 0;                         // used to calculate integration interval
uint32_t Now2 = 0;
uint32_t shift_dt = 0;

uint32_t periodTime = 20000; // период осциллятора
uint32_t duty = 15;           // 15 % 
uint32_t fall = 0;

uint32_t shift = 0;

uint32_t phase12 = 50;

boolean enable_shim = 0;
int shim = 1;

uint32_t sync_phase = 4000;
uint32_t sync_pulse = 4000;


volatile int count= 0;
volatile int count1 = 0;
int ncount = 0;

int a0,a1,a2,a3; // A3 - это частота вверх-вниз левый джойстик. A2 - скважность вправо-влево левый джойстик.
int r0=0,r1=0,r2=0,r3=0;
int inc0 = 100;

#define  CNT  10



void setup(){
  
  //set pins as outputs
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);

   time_sec = 0.0;
  toggle0 = 1;

  Serial.begin(9600);

cli();//stop interrupts
/*
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 2;//124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 <<CS10);//(1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
*/
/*  
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1;//15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= /*(1 << CS12) | *-/(1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
*/
/*
//set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
/*/

sei();//allow interrupts


}//end setup
/*
ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
  /*if (toggle0){
    digitalWrite(10,HIGH);
    //PORTB = 0x40;
    toggle0 = 0;
  }
  else{
    digitalWrite(10,LOW);
    //PORTB |= ~0x40;
    toggle0 = 1;
  }* /

  currTime ++;
  if (currTime == 0xFFFFFFFF) {currTime = 0; }
}
*/
/*
ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle1){
    digitalWrite(9,HIGH);
    OCR1A = 10;
    toggle1 = 0;
  }
  else{
    digitalWrite(9,LOW);
    OCR1A = 1;
    toggle1 = 1;
  }
}
*/
/*
ISR(TIMER2_COMPA_vect){//timer1 interrupt 8kHz toggles pin 9
//generates pulse wave of frequency 8kHz/2 = 4kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle2){
    digitalWrite(10,HIGH);
    toggle2 = 0;
  }
  else{
    digitalWrite(10,LOW);
    toggle2 = 1;
  }
}
*/

void print_a1(){
  a0 = analogRead(A0);
  a1 = analogRead(A1);
  /*Serial.print("P=");
  Serial.print(sync_pulse);
  Serial.print("F=");
  Serial.println(sync_phase);
*/
  if (a0 > 700) 
  {
    inc0 = 5;
    
    if (r0 > CNT)
    {
      // уменьшаем длительность импульса
      if (sync_pulse >= (100+inc0)) //500 Hz
      {
        sync_pulse-=inc0;
        r0 = 0;
      }
    }else
    {
      r0++; 
    }
  } else
  if (a0 < 300) 
  {
    inc0 = 5;
    
    if (r0 > CNT)
    {
      // увеличиваем период
      if (sync_pulse <= (periodTime/4 - inc0) ) // 1Hz 
      {
        sync_pulse +=inc0;
        r0 = 0;
      }
    }else
    {
      r0++; 
    }
  }else {
    r0 = 0;
  }
  //Serial.print("D=");
  //Serial.print(duty);

  // Период
  if (a1 > 700) 
  {
    inc0 = 5;
    //if (a1 > 1000) inc0 = 50;
    if (r1 > CNT)
    {
      // уменьшаем период
      if (sync_phase >= (0+inc0)) //500 Hz
      {
        sync_phase -= inc0;
        r1 = 0;
      }
    }else
    {
      r1++; 
    }
  } else
  if (a1 < 300) 
  {
    inc0 = 5;
    //if (a1 < 24) inc0 = 50;
    if (r1 > CNT)
    {
      // увеличиваем период
      if ( sync_phase <= (periodTime/2-inc0) ) // 1Hz 
      {
        sync_phase += inc0;
        r1 = 0;
      }
    }else
    {
      r1++; 
    }
  }else {
    r1 = 0;
  }


}

void print_a2(){
  a0 = analogRead(A2);
  a1 = analogRead(A3);
  /*Serial.print("A2=");
  Serial.print(a0);
  Serial.print("A3=");
  Serial.println(a1);*/

  // Скважность

  if (a0 > 700) 
  {
    inc0 = 1;
    
    if (r2 > 8*CNT)
    {
      // уменьшаем скважность
      if (duty >= (4+inc0)) //500 Hz
      {
        duty-=inc0;
        r2 = 0;
      }
    }else
    {
      r2++; 
    }
  } else
  if (a0 < 300) 
  {
    inc0 = 1;
    
    if (r2 > 8*CNT)
    {
      // увеличиваем период
      if (duty <= (48-inc0) ) // 1Hz 
      {
        duty +=inc0;
        r2 = 0;
      }
    }else
    {
      r2++; 
    }
  }else {
    r2 = 0;
  }
  //Serial.print("D=");
  //Serial.print(duty);

  // Период
  if (a1 > 700) 
  {
    inc0 = 5;
    if (a1 > 1000) inc0 = 50;
    if (r3 > CNT)
    {
      // уменьшаем период
      if (periodTime >= (20000+inc0)) //500 Hz
      {
        periodTime-=inc0;
        r3 = 0;
      }
    }else
    {
      r3++; 
    }
  } else
  if (a1 < 300) 
  {
    inc0 = 5;
    if (a1 < 24) inc0 = 50;
    if (r3 > CNT)
    {
      // увеличиваем период
      if (periodTime <= (1000000-inc0) ) // 1Hz 
      {
        periodTime+=inc0;
        r3 = 0;
      }
    }else
    {
      r3++; 
    }
  }else {
    r3 = 0;
  }

  //Serial.print("P=");
  //Serial.println(periodTime);
}

void check_shim(){
  shim = digitalRead(2);
  if (shim) enable_shim = 0;
  else enable_shim = 1;
}


void loop(){
  //do other things here
  //
  // read buttons and adjust frequency.
  // 
  shift = periodTime * phase12 / 100 + 0.75*(double)shift_dt;


  Now = micros();
  deltat = (Now - lastUpdate);
  lastUpdate = Now;
  time_sec += deltat;
  time_sec2 += deltat;
   
  fall = periodTime * duty / 100;
  check_shim();
  print_a1();

  if ((time_sec > 0) && (time_sec < fall))
  {
    if (toggle0 == 0 || !enable_shim) 
      {
        digitalWrite(9, HIGH);
        if (count1 > ncount){
          toggle0 = 1;
          count1 = 0;
        }else
        {
          count1++;
        }
      } else {
        digitalWrite(9, LOW);
        if (count1 > ncount){
          toggle0 = 0;
          count1 = 0;
        } else
        {
          count1++;
        }
      }
      
  } 
  else 
  if ((time_sec >= fall) && (time_sec < periodTime ))
  {
    toggle0 = 0;
    count1 = 0;
    digitalWrite (9, LOW);
    
  }
  else 
  if (time_sec >= periodTime)
  {
    time_sec = 0;
    time_sec2 = shift;
  }

  // syncro pulse
  if ( (time_sec > sync_phase ) && (time_sec < sync_phase + sync_pulse) )  {
    digitalWrite(8,HIGH);
  }
  else 
  if ( (time_sec >= sync_phase + sync_pulse) && (time_sec2 <= sync_phase )){
    digitalWrite(8, LOW );
  }else 
  if ( (time_sec2 > sync_phase ) && (time_sec2 < sync_phase + sync_pulse) )  {
    digitalWrite(8,HIGH);
  }
  else{
    digitalWrite(8, LOW );
  }

  print_a2();

  // second pulse
  Now2 = micros();
  shift_dt = Now2 - Now;



  if ( time_sec2 > 0 && time_sec2 < fall )
  {
    if (toggle1 == 0 || !enable_shim ) 
    {
      digitalWrite(10, HIGH);
      //PORTB |= 0x40;
      if (count > ncount){
        toggle1 = 1;
        count = 0;
      }else
      {
        count++;
      }
    }
    else
    {
      if (count >ncount){
        toggle1 = 0;
        count = 0;
      }else
      {
        count++;
      }
       //PORTB &= ~0x40;
       digitalWrite(10, LOW); 
    }
  } 
  else 
  if ( time_sec2 >= fall && time_sec2 < periodTime  ) 
  {
    toggle1 = 0;
    //if (toggle0 == 0)
    //{
      digitalWrite (10, LOW);
      //PORTB &= ~0x40;
      
            //toggle0 = 1;
    //}
    
  }else 
  if (time_sec2 > periodTime)
  {
    time_sec2 = 0;
    count = 0;
  }

  
  
}
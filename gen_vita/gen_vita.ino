// Генератор  на 1 кГц - пин 10
// Генератор на 50 Гц - пин 8
// Задающий импульс 50 Гц, сдвиг фаз относительно восходящего фронта 50Гц генератора
// определяется величиной переменной sync_phase (микросекунды) [0..400]
// ширина синхроимпульса задается переменной sync_pulse (микросекунды) [20..80]
//
//
//storage variables
volatile boolean toggle0 = 0;


uint32_t periodTime = 1000; // период осциллятора в микросекундах
uint32_t sync_phase = 40;
uint32_t sync_pulse = 20;


volatile int count= 0;
volatile int count1 = 0;
int ncount = 10;

int a0,a1,a2,a3; // A3 - это частота вверх-вниз левый джойстик. A2 - скважность вправо-влево левый джойстик.
int r0=0,r1=0,r2=0,r3=0;
int inc0 = 100;

#define  CNT  10



void setup(){
  
  //set pins as outputs
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);

  Serial.begin(9600);

  count = 0;
  count1 = 0;
}//end setup



void loop(){
   
  //Now = micros();
  //deltat = (Now - lastUpdate);
  
  if (!toggle0){
    digitalWrite(9, HIGH);
    if (count == 40) { digitalWrite(10, LOW); count = 0; }
    if (count == 20) { 
      digitalWrite(10, HIGH);
      delayMicroseconds( sync_phase );
      digitalWrite(8, HIGH);
      delayMicroseconds( sync_pulse );
      digitalWrite(8, LOW);
      delayMicroseconds(periodTime/2 - sync_pulse - sync_phase - 8); // 8 микросекунд на вызовы функций (примерно)
    }else {
      delayMicroseconds(periodTime/2);
    }
    
    toggle0 = 1;
  } else
  if (toggle0){
    digitalWrite (9, LOW);
    
    toggle0 = 0;
    delayMicroseconds(periodTime/2);

  }
  count ++;
    
  
}

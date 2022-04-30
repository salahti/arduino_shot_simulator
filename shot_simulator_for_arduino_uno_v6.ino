/*
  Arduino shot simulator
  This sketch is used to simulate pellet hitting Inband Air electronic target
  pin10=mic4
  pin11=mic3
  pin12=mic2
  pin13=mic1
*/

//constants


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(10, OUTPUT);//connect to microphones
  pinMode(11, OUTPUT);//connect to microphones
  pinMode(12, OUTPUT);//connect to microphones
  pinMode(13, OUTPUT);//microphone and also led
  pinMode(2,INPUT_PULLUP);//Button
  pinMode(3,INPUT_PULLUP);//Button
  
  digitalWrite(10,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(13,HIGH);
  delay(5000);  //
  Serial.begin(115200,SERIAL_8N1); //UART going to USB com port
  
}

// the loop function runs over and over again forever
void loop() {

  //shoot_line();
  //shoot_static();
  //shoot_circle();
  //shoot_h_line();
  check_buttons();
}

void check_buttons(){
    int val=digitalRead(2);
     if (val==LOW) single_random();

    val=digitalRead(3);
    if (val==LOW) random_shots();
    
  
}



void random_shots(){
  int a;
  float x,y;
  int maxi=150; //max dif in tents of mm, i.e 50 equals 5mm
  int min=20;
  for (int n=0;n<10;n++){
    a=random((0,maxi));
    a=a-maxi/2;
    x=a/10;
    a=random((0,maxi));
    a=a-maxi/2;
    y=a/10;
    calc_delays(x,y);
    delay(300);
    
  }
}

void single_random(){
  int a;
  float x,y;
  int maxi=150; //max dif in tents of mm, i.e 50 equals 5mm
  int min=20;

    a=random((0,maxi));
    a=a-maxi/2;
    x=a/10;
    a=random((0,maxi));
    a=a-maxi/2;
    y=a/10;
    calc_delays(x,y);
    delay(50);
    
}

void shoot_circle(){

  float r=10.0; //radious of the circle
  float pi=3.14; 
  float a=0; // angle to be looped
  float s=0.10472; //angle step in radians
  float x,y;
  for (int n=0;n<60;n++){
    x=cos(a)*r;
    y=sin(a)*r;
    calc_delays(x,y);
    delay(2000);
    a=a+s;
    
    
  }
  
}

//shoots horizontal line
void shoot_h_line(){
  float x_start=-10.0;
  float y_start=-10.0;
  float ste=abs(2*x_start/100);
  for (int n=0; n <100; n++){
     calc_delays(x_start,y_start);
     x_start=x_start+ste;
     //y_start=y_start+ste;
     delay(2000);
  }}

void shoot_line(){
  float x_start=-20.0;
  float y_start=-20.0;
  float ste=0.5;
  for (int n=0; n <60; n++){
     calc_delays(x_start,y_start);
     x_start=x_start+ste;
     y_start=y_start+ste;
     delay(2000);
  }}

void shoot_static(){
  for (int n=0; n <1000; n++){
     calc_delays(-5.2,-7.0);
     
     delay(2000);
}}

//calc_delays calculates needed delays based on wanted hit coordinates x and y
void calc_delays(float x, float y){
  //constants
  double v=344.0;
  float X=100.12; //default calibration, positive or negative value depending on mic
  float Y=115.0;  //close enough value to be used in Y, enough to get roughly right coordinates
  float a=625; //how many delay tics per one ms

  //distance from hit to Mic 1
  double s1,s2,s3,s4;
  s1=sqrt(pow((x-(-X)),2)+pow((y-(-Y)),2));
  s2=sqrt(pow((x-(X)),2)+pow((y-(-Y)),2));
  s3=sqrt(pow((x-(X)),2)+pow((y-(Y)),2));
  s4=sqrt(pow((x-(-X)),2)+pow((y-(Y)),2));

  // calculated corresponding times in ms

  s1=s1/v;
  s2=s2/v;
  s3=s3/v;
  s4=s4/v;

  //calculate corresponding delays

  s1=s1*a;
  s2=s2*a;
  s3=s3*a;
  s4=s4*a;

  int del1=s1;
  int del2=s2;
  int del3=s3; 
  int del4=s4;
  
  //send to serial port for debug purposes
  //some known values just for checking the serial port functioning
  /*
  del1=100;
  del2=300;
  del3=500; 
  del4=700
  */
  
  byte tmp_low=lowByte(del1);
  byte tmp_high=highByte(del1);
  Serial.write(tmp_low);
  Serial.write(tmp_high);

  tmp_low=lowByte(del2);
  tmp_high=highByte(del2);
  Serial.write(tmp_low);
  Serial.write(tmp_high);

  tmp_low=lowByte(del3);
  tmp_high=highByte(del3);
  Serial.write(tmp_low);
  Serial.write(tmp_high);

  tmp_low=lowByte(del4);
  tmp_high=highByte(del4);
  Serial.write(tmp_low);
  Serial.write(tmp_high);

  trigger_mics(del1,del2,del3,del4);
  
  
}

void trigger_mics(int del1,int del2,int del3, int del4) {
  //del1...del4 are delays for triggering mics 1...4
  //reset mic lines
  digitalWrite(10,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(13,HIGH);
  //mic statusflags, used to make sure IO transitions are not done un-necessary
  bool M1=0;
  bool M2=0;
  bool M3=0;
  bool M4=0;

 //compensate for delay
 int foo=0;
  
  int n=0;
  noInterrupts();
   for (n=0; n <5000; n++){
   if (M1==0 && n>del1)
    {digitalWrite (13,LOW);
     M1=1;
     //n=n+foo;
     } //mic 1
     
   if (M2==0 && n>del2){
    digitalWrite (12,LOW);
    M2=1;
    //n=n+foo;
    } //mic 2
   if (M3==0 && n>del3){
    digitalWrite (11,LOW);
    M3=1;
    //n=n+foo;
    } //mic 3
   if (M4==0 && n>del4){
    digitalWrite (10,LOW);
    M4=1;
    //n=n+foo;
    }//mic4
    if (M1==1 && M2==1 && M3==1 && M4==1){break;
   //dum_delay(5);
   }}
  interrupts(); 
  delay(300);
  digitalWrite(10,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(13,HIGH); 
}


void dum_delay(int delay) {
  //two loops doing nothing, inner repeats 1000

   for (int n=0; n <delay; n++){
   for (int i=0; i <1000; i++){
    __asm__("nop\n\t"); 
}}

   }

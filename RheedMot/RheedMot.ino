// encoder related variables
long posEncoder=0;

// motor related variables
const int pin0=4;
long posMotor=0,posMotorEnd=0,posReport=0;
int pin=0; 
unsigned long report = millis();
//---------------------------------------------

void setup() 
{          
  for (int i=0;i<4;i++){
    pinMode(pin0+i, OUTPUT);     
    digitalWrite(pin0+i, LOW);
  }
  // setup encoder related pins
  pinMode(2, INPUT);
  digitalWrite(2,HIGH);
  pinMode(3, INPUT);
  digitalWrite(3,HIGH);
  
  pinMode(11, OUTPUT);     
  pinMode(12, OUTPUT);
  
  Serial.begin(9600);
  Serial.setTimeout(20);
  Serial.println("connected");
}
//---------------------------------------------

void loop() 
{
  digitalWrite(11,!digitalRead(2));
  digitalWrite(12,!digitalRead(3));
  
  int enc=2*readEncoder();
  posMotorEnd+=enc;
  posReport+=enc;

  if (posMotor!=posMotorEnd)
  {
    int dir=(posMotor<posMotorEnd)*2-1;
    pin=(pin+dir+256)%4; 

    digitalWrite(pin0+pin, HIGH);   
    delayMicroseconds(2500);
    digitalWrite(pin0+pin, LOW);
    posMotor+=dir;
  }else{
    if(millis() - report >= 100){
      Serial.println(posMotor);
      report = millis();
    }
  }
}
//---------------------------------------------

  void serialEvent(){
    int cmd = Serial.readString().toInt();
    if (cmd != 0){
      posMotorEnd+=cmd; 
    }else{ 
      posMotorEnd=posMotor;
    }
  }
//---------------------------------------------

// returns change in encoder state (-1,0,1)
int readEncoder()
{
  static int enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static byte old_AB = 0;
  old_AB <<= 2;                   //remember previous state
  old_AB = old_AB | (digitalRead(2)<<1) | digitalRead(3);
  int enc=enc_states[old_AB & 0x0f];
  posEncoder+=enc;
  return enc;
}

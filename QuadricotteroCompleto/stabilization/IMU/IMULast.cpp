#include "IMULast.h"
#include "sensor/WMP.h"

WMP wmp;

void IMULast::start(){
  //wmp.init();
  
  initAcc();
}

unsigned long time12, sumTimePart0=0, sumTimePart1=0;
int updateNumber=0;
void IMULast::update(unsigned long){
  time12 = micros();
  updateNumber++;
  
  wmp.update();
  
  sumTimePart0+= micros()-time12;
  time12 = micros();
	
  yaw=analogRead(0)-midA;
  pitch=analogRead(1)-midB;
  roll=analogRead(2)-midC;
  
  sumTimePart1+= micros()-time12;
}

float* IMULast::leggi(float ris[]){
  //float ris[6];
  
  float *tRis = wmp.leggi();
  for (int i=0;i<3;i++){
    ris[i]=tRis[i];
  }
  
  ris[3]= yaw;
  ris[4]= pitch;
  ris[5]= roll;
  return ris;
}

void IMULast::debug(){
  
  Serial.print("Part0:");
  Serial.println(sumTimePart0/updateNumber);
  
  Serial.print("Part1:");
  Serial.println(sumTimePart1/updateNumber);
  
  sumTimePart0 = sumTimePart1 = updateNumber=0;
  wmp.debug();
}

void IMULast::initAcc(){
  unsigned long time1 = millis()+1000;
  unsigned long a =0, b=0, c=0;
  int count =0;
  while( time1>millis() ){
    count++;
    a+=analogRead(0);
    b+=analogRead(1);
    c+=analogRead(2);
    //            delayMicroseconds(222);
  }

  a/=count;
  midA=a;
  Serial.print("MID acc A ");
  Serial.print( a );
  Serial.print(" ");

  b/=count;
  midB=b;
  Serial.print("MID acc B ");
  Serial.print( b );
  Serial.print(" ");

  c/=count;  
  midC=c;
  Serial.print("MID acc C ");
  Serial.print( c );
  Serial.print(" ");
  
  int chosen;
  if (midA>midB && midA>midC){
    chosen = midA=(midB+midC)/2;
  }else if (midB>midA && midB>midC){
    chosen = midB=(midA+midC)/2;
  }if (midC>midB && midC>midA){
    chosen = midC=(midA+midB)/2;
  }else{
    Serial.print("ERROR: ALL ACCELEROMETER DATA IS EQUAL!");
  }
  
  Serial.print("MID acc chosen ");
  Serial.print( chosen );
  
  Serial.print("\t");
  Serial.print( count );
  Serial.println();
}
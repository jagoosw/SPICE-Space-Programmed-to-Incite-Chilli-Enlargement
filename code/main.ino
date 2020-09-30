
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float co,error,dev_error,int_error;
int zero_position=0;
float cycle_time=200;
int record_time=10;
float history[100];
int i,j;
float start_time=16;//This is a work around, I want to be able to send it the current time at start up but this is much less complex


void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  pinMode(2, OUTPUT);
  float history[record_time];
  for(i=0;i<record_time;i++){
    history[i]=bmp.readTemperature();
    delay(100);
  }
}

float get_int_error(float time_frame, float target, float bias){
  float int_error=0;
  for(i=zero_position+time_frame;i>zero_position;i=i-1){
    if(i<0){
      j=record_time+i;
    }else if(i>record_time){
      j=record_time-record_time;
    }
    int_error = int_error+(target+bias-history[j]);
  }
  return int_error/time_frame;
}

float get_dev_error(float time_frame, float target, float bias){
  float dev_error=0;
  for(i=zero_position+time_frame;i>zero_position;i=i-1){
    if(i<0){
      j=record_time+i;
    }else if(i>record_time){
      j=record_time-record_time;
    }
    dev_error = dev_error+(target+bias-history[j])/(zero_position-i);
  }
  dev_error=dev_error/time_frame;
  return dev_error;
}
int heat_time(float target,float k_p,float k_i,float k_d,float bias){
  //The control loop, will impliment with P first the add D then I
  error = target+bias-history[zero_position];
  dev_error=0;
  co=k_p*error+k_i*get_int_error(10,target,bias)+k_d*get_dev_error(10,target,bias);
  return co;
}
int count=0;
float target;
float k_u=50;
float t_u=14;
float k_p=0.6*k_u;
float k_i=1.2*k_u/t_u;
float k_d=3*k_u*t_u/40;
float bias = 4;

void loop() {
  history[zero_position]=bmp.readTemperature();
  if(7<((int(millis()/3600000+start_time)) % 24)<19){
    target=30;
  }else{
    target=20;
  }
  Serial.println(target);
  co=heat_time(target,k_p,k_i,k_d,bias);
  if(co<0){
    delay(cycle_time);
    Serial.println(history[zero_position]);
    //Serial.print("\t");
    //Serial.println(co/100);
  }else{
    digitalWrite(2, HIGH);
    Serial.println(history[zero_position]);
    //Serial.print("\t");
    //Serial.println(co/100);
    
    delay(co); 
    digitalWrite(2, LOW);
    if(co<=cycle_time){
      delay(cycle_time-co); 
    }
  }
  //Serial.print("\t");
  //Serial.println(count);
  count++;
    if(zero_position>=record_time){
    zero_position=0;
  }
  zero_position++;
}

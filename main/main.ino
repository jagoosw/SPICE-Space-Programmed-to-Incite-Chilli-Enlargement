
#include <dht.h>

#define dht_pin 3
 
dht DHT;

#include <Arduino.h>
#include <OLED_SSD1306_Chart.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

OLED_SSD1306_Chart display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float co,error,dev_error,int_error;
int zero_position=0;
float cycle_time=200;
int record_time=10;
float history[100];
int i,j;
float start_time=16;//This is a work around, I want to be able to send it the current time at start up but this is much less complex


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  float history[record_time];
  for(i=0;i<record_time;i++){
    history[i]=DHT.temperature;
    delay(100);
  }
//Screen setup
  #if defined ESP8266
  #else
  Wire.begin();
  #endif

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setChartCoordinates(0, 60);         //Chart lower left coordinates (X, Y)
  display.setChartWidthAndHeight(123, 60);    //Chart width = 123 and height = 60
  display.setXIncrement(2);                   //Distance between Y points will be 5px
  display.setYLimits(25, 45);                 //Ymin = 0 and Ymax = 100
  display.setAxisDivisionsInc(10, 30);         //Each 12 px a division will be painted in X axis and each 6px in Y axis
  display.setPlotMode(SINGLE_PLOT_MODE);      //Set single plot mode
  display.drawChart();                        //Update the buffer to draw the cartesian chart 
  display.display();
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
float bias = -1;

void loop() {
  DHT.read11(dht_pin);
  history[zero_position]=DHT.temperature;
  if(7<((int(millis()/3600000+start_time)) % 24)<19){
    target=40;
  }else{
    target=20;
  }
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
  if(!display.updateChart(history[zero_position])){
      display.clearDisplay();                 //If chart is full, it is drawn again
      display.drawChart();
  }
  //Serial.print("\t");
  //Serial.println(count);
  count++;
  if(zero_position>=record_time){
    zero_position=0;
  }
  zero_position++;
}

#include "DHT.h"
#include <Arduino.h>
#include <OLED_SSD1306_Chart.h>

#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
OLED_SSD1306_Chart display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float co, error, dev_error, int_error, target;
int zero_position = 0;
int i, j, step_count;

int cycle_time = 2000;
float co_max = 200;
int record_time = 2;
float history[2];
float start_time = 16; //This is a work around, I want to be able to send it the current time at start up but this is much less complex
int check_count = 0;
unsigned long check_time = 0;

float k_u = 50; //This needs retuning
float t_u = 14;
float k_p = 60; //0.6*k_u;
float k_i = 0; //1.2*k_u/t_u;
float k_d = 0; //3*k_u*t_u/40;
float bias = 0;

int poll_interval = 2000; //The maximum polling speed of the sensor is ~2 seconds

void setup() {
  Serial.begin(9600);

  dht.begin();
  pinMode(2, OUTPUT);

  float history[record_time];

  Serial.println("Starting");

  while (check_count < record_time) {
    if (millis() > check_time + 5000) {
      history[check_count] = dht.readTemperature();
      check_count++;
      check_time = millis();
    }
  }

  delay(10000);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setChartCoordinates(0, 60);         //Chart lower left coordinates (X, Y)
  display.setChartWidthAndHeight(123, 60);    //Chart width = 123 and height = 60
  display.setXIncrement(2);                   //Distance between Y points will be 5px
  display.setYLimits(15, 45);                 //Ymin = 0 and Ymax = 100
  display.setAxisDivisionsInc(10, 30);         //Each 12 px a division will be painted in X axis and each 6px in Y axis
  display.setPlotMode(SINGLE_PLOT_MODE);      //Set single plot mode
  display.drawChart();                        //Update the buffer to draw the cartesian chart
  display.display();
}

float get_int_error(float time_frame, float target, float bias) {
  float int_error = 0;
  for (i = zero_position + time_frame; i > zero_position; i = i - 1) {
    if (i < 0) {
      j = record_time + i;
    } else if (i > record_time) {
      j = record_time - record_time;
    }
    int_error = int_error + (target + bias - history[j]);
  }
  return int_error / time_frame;
}

float get_dev_error(float time_frame, float target, float bias) {
  float dev_error = 0;
  for (i = zero_position + time_frame; i > zero_position; i = i - 1) {
    if (i < 0) {
      j = record_time + i;
    } else if (i > record_time) {
      j = record_time - record_time;
    }
    dev_error = dev_error + (target + bias - history[j]) / (zero_position - i);
  }
  dev_error = dev_error / time_frame;
  return dev_error;
}

int heat_time(float target, float k_p, float k_i, float k_d, float bias) {
  //The control loop, will impliment with P first the add D then I
  error = target + bias - history[zero_position];
  co = k_p * error + k_i * get_int_error(10, target, bias) + k_d * get_dev_error(10, target, bias);
  return co / 10;
}

void loop() {
  if (millis() > check_time + poll_interval) {
    history[zero_position] = dht.readTemperature();
    if (7 < ((int(millis() / 3600000 + start_time)) % 24) < 19) {
      target = 30;
    } else {
      target = 20;
    }
    co = heat_time(target, k_p, k_i, k_d, bias);
    check_time = millis();
    Serial.println(history[zero_position]);
    if (!display.updateChart(history[zero_position])) {
      display.clearDisplay();
      display.drawChart();
    }
    zero_position++;
    if (zero_position >= record_time) {
      zero_position = 0;
    }
    if (check_time > 4294967295 - poll_interval) {
      check_time = 0;
    }
  }
  if (co < 0) {
    delay(cycle_time);
  } else {
    if (co <= co_max) {
      for (step_count = 0; step_count <= cycle_time / 200; step_count++) {
        digitalWrite(2, HIGH);
        delay((cycle_time / 200)*co / co_max);
        digitalWrite(2, LOW);
        delay((cycle_time - 1) / 200 * co / co_max);
      }
    }
    else {
      digitalWrite(2, HIGH);
      delay(cycle_time);
      digitalWrite(2, LOW);
    }
  }
}

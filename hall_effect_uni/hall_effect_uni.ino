
// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#include <ros.h>
#include <std_msgs/Float32.h>
#define MAX_ANALOG_INPUT_VAL 1023
#define Hall_Sensor A0          //A0 used with analog output, D2 with digital output
#define Hall_Sensor_D 2

ros::NodeHandle  nh;
std_msgs::Float32 hall_eff;
ros::Publisher chatter("chatter", &hall_eff);
//const int LED_OUTPUT_PIN = LED_BUILTIN;



// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.

const int SMOOTHING_WINDOW_SIZE = 50; // 10 samples

const int numReadings=5;
int readings[numReadings];            //Number of readings
float _samples[SMOOTHING_WINDOW_SIZE];  // the readings from the analog input
int _curReadIndex = 0;                // the index of the current reading
float _sampleTotal = 0;                 // the running total
float _sampleAvg = 0;                   // the average
float newAv = 0;
float sensorVal = 0;
float maxV = 0;
void setup() {

//  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(Hall_Sensor,INPUT);
  
  Serial.begin(115200);
  nh.initNode();
  nh.advertise(chatter);
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++) {
    _samples[i] = 0;
  }
  for (int i=0 ;i<numReadings;i++)
  readings[i]=analogRead(A0);//  
  maxV = readings[numReadings-1];
  analogWriteResolution(12);
}

void loop() {
      int startT = micros();
  // subtract the last reading:
  _sampleTotal = _sampleTotal - _samples[_curReadIndex];
  // read the sensor value
  sensorVal = analogRead(Hall_Sensor);
//  if(startT >= 30000 && startT <= 70000){
//    dummy = sensorVal;
//    startT = 0;
//  }

  _samples[_curReadIndex] = sensorVal;
  
  // add the reading to the total:
  _sampleTotal = _sampleTotal + _samples[_curReadIndex];
  
  // advance to the next position in the array:
  _curReadIndex = _curReadIndex + 1;

  // if we're at the end of the array...
  if (_curReadIndex >= SMOOTHING_WINDOW_SIZE) {
    // ...wrap around to the beginning:
    _curReadIndex = 0;
  }

  // calculate the average:
  _sampleAvg = _sampleTotal / SMOOTHING_WINDOW_SIZE;
  
  newAv = abs(_sampleAvg - maxV);  
  hall_eff.data = newAv;
  Serial.print("Smoothed:");
  Serial.println(newAv);
//  Serial.println(s/tartT);
  chatter.publish( &hall_eff );
  nh.spinOnce();
  delay(2);
}

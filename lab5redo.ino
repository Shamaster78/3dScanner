#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "Adafruit_VL6180X.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *stepup = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *steprad = AFMS.getStepper(200, 1);
Adafruit_VL6180X vl = Adafruit_VL6180X();

void motorStep(){
  stepup->step(10,FORWARD,MICROSTEP);
}

#define limswitch 8 // initialize limit switch to pin 8
static int stepturn = 0; // set inital state of rotating stepper
static int stepmax = 200; // 1 revolution for spinning plate
static int h = 0;
static int steph = 0; // vertical stepper motor set to 0
static int switchState=0; // set initial state of NC limit switch to 0
static int startScan=0; // counter to help initialize scan
static int hmax = 200; // set height of object 

void setup() {
  Serial.begin(9600);                 // initialize serial monitor
  pinMode(limswitch, INPUT_PULLUP); // sets limit switch using built in Arduino resistor
  AFMS.begin();                       // create with the default frequency 1.6KHz
  
  stepup->setSpeed(100);       // 20 rpm   
  // homing
  switchState = digitalRead(limswitch);
  while(switchState == 0){
    Serial.print("This shit homin' homie, limit switch state = \t"); // degubber to check switch
    switchState = digitalRead(limswitch);
    delay(10);
    stepup->step(1, FORWARD, MICROSTEP);
    Serial.println(switchState);
    
  }
  if(switchState == 1){
    stepup->step(5, BACKWARD, MICROSTEP);
    Serial.println("Homing complete"); 
    ++startScan;
    }
while (!Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");
}

void loop() 
{
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  Serial.print("Lux: "); Serial.println(lux);
  
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) {
    Serial.print("Range: "); Serial.println(range);
  }

  // Some error occurred, print it out!
  
  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    Serial.println("System error");
  }
  else if (status == VL6180X_ERROR_ECEFAIL) {
    Serial.println("ECE failure");
  }
  else if (status == VL6180X_ERROR_NOCONVERGE) {
    Serial.println("No convergence");
  }
  else if (status == VL6180X_ERROR_RANGEIGNORE) {
    Serial.println("Ignoring range");
  }
  else if (status == VL6180X_ERROR_SNR) {
    Serial.println("Signal/Noise error");
  }
  else if (status == VL6180X_ERROR_RAWUFLOW) {
    Serial.println("Raw reading underflow");
  }
  else if (status == VL6180X_ERROR_RAWOFLOW) {
    Serial.println("Raw reading overflow");
  }
  else if (status == VL6180X_ERROR_RANGEUFLOW) {
    Serial.println("Range reading underflow");
  }
  else if (status == VL6180X_ERROR_RANGEOFLOW) {
    Serial.println("Range reading overflow");
  }
  delay(50);
  
if(startScan == 1)
  {
  Serial.println("Object detected, begin scan");
  for(h < hmax; ++h;)
    {
     stepup->step(1, FORWARD, SINGLE);
  for(stepturn < stepmax; ++stepturn;)
    {
      steprad->step(1, FORWARD, SINGLE);
      Serial.println(h);  // sensor's vertical position
      Serial.print(",");
      Serial.print(stepturn);  // objects rotational position
      Serial.print(",");
      Serial.print(range);  // sensor's value
    }
    stepturn = 0;
    Serial.println("Height increased!");
    }
    Serial.println("Scan complete!");
  }
 else
  Serial.println("Oops, broke");

}

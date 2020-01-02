#include <Wire.h>
#include <Servo.h>

//Pin 6 - Servo Signal
//Pin 2 - Encoder Signal In
//Pin A5 - SCL
//Pin A4 - SDA

const int encRawLowVal = 32; // 32 microseconds
const int encRawHighVal = 1040; // 1040 microseconds
//const int encRawMiddle = (encRawHighVal+encRawLowVal)/2;
const int measuredAcc = 4; // Can only measure in increments of 4 microseconds
// Input value can be from 0 to 252
const int encLowVal = 0;
const int encHighVal = 252;
const int encMidVal = 126; //(encHighVal + 1) / 2;

const int ccLow = 1520; // Counter Clockwise Low Value in micro seconds;
const int ccHigh = 1720; // Counter Clockwise High Value in micro seconds;
const int cLow = 1280; // Clockwise Low Value in micro seconds;
const int cHigh = 1480; // Clockwise High Value in micro seconds;
const int stopLow = cHigh; // Stop Low Value in micro seconds;
const int stopHigh = ccLow; // Stop High Value in micro seconds;
const int servoIncrements = 10; // Increments that the servo can read in micro seconds

const int kP = 9; // PID - P value
const int maxError = 30;

volatile unsigned long prevRawEncoder = 0;
volatile byte encoderValueWrap = 0;
volatile int encoderValueWind = 0;
volatile int windCount = 0;
volatile int prev_time = 0;
volatile int goal = 0;

Servo servo;


//
//  Input: Value of 0 is stopped. Negative values are Clockwise. Positive values are Counter Clockwise
//  Takes a value and turns it into a PW value that represents the servo speed.
//
int servoControl(int value)
{
  int servoControl = 1500; //((stopHigh - stopLow)/2) + stopLow;
  
  if (value > (ccHigh-ccLow)) // Value is the most positive, go counter clockwise
  {
    servoControl = ccHigh;
  }
  else if (value < (cLow - cHigh))  // Value is the most negative, go counter clockwise
  {
    servoControl = cLow;
  }
  else if (value > 0) // Value is positive, go counter clockwise
  {
    servoControl = value + ccLow;
  }
  else if (value < 0) // Value is negative, go clockwise
  {
    servoControl = value + cHigh;
  }
  return servoControl;
}
 
void setup() {
//  Serial.begin(115200);
  
  Wire.begin(16);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
  
  // when pin D2 goes high, call the rising function
  attachInterrupt(0, rising, RISING);
  
  servo.attach(6);
  //servo.writeMicroseconds(1500);
}

void loop()
{
  int error = encoderValueWind - goal;

  int power;
  if (error <= -maxError)
  {
    power = -8000;
  }
  else if (error >= maxError)
  {
    power = 8000;
  }
  else
  {
    power = error * kP;
  }

//  int maxSpeed = 40;
//  if (power > maxSpeed)
//  {
//    power = maxSpeed;
//  }
//  else if (power < (-maxSpeed))
//  {
//    power = -maxSpeed;
//  }
  
  servo.writeMicroseconds(servoControl(power));
//  Serial.println(goal, DEC);
}

void requestEvent()
{
  // Send the wrap value for 0-360 like measurement
  Wire.write(encoderValueWrap);
}

int mod(int x, int m) {
    return (x%m + m)%m;
}

void receiveEvent(int numBytes)
{
  if (numBytes == 0)
    return;

  byte address = Wire.read();
  
  if (address == 0 && numBytes == 2)
  {
    byte byte0 = Wire.read();
    if (byte0 > encHighVal)
    {
      return;
    }

//    Serial.print(goal, DEC);
//    Serial.print(", ");
//    Serial.print(byte0, DEC);
//    Serial.print(", ");
    
    int diff = (goal - byte0);
    int mod360 = mod(diff, encHighVal);
    int mod180 = mod(diff, encMidVal);

//    Serial.print(mod360, DEC);
//    Serial.print(", ");
//    Serial.print(mod180, DEC);
//    Serial.print(", ");

    // Pick the fastest route
    if (mod360 >= encMidVal)
    {
      goal += (encMidVal-mod180);
    }
    else
    {
      goal -= mod180;
    }
//    Serial.println(goal, DEC);
  }
  else
  {
    while (Wire.available())
    {
      byte dump = Wire.read();
    }
  }
  //Serial.println(prevRawEncoder, DEC);
}

void rising() {
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}
 
void falling() {
  attachInterrupt(0, rising, RISING);
  word rawEncoderValue = micros()-prev_time;

  // TODO: Investigate Spikes in position
  if (rawEncoderValue != prevRawEncoder)
  {
    if (rawEncoderValue <= encRawLowVal)
    {
      encoderValueWrap = encLowVal;
    }
    else if (rawEncoderValue >= encRawHighVal)
    {
      encoderValueWrap = encHighVal;
    }
    else
    {
      unsigned long temp = rawEncoderValue - encRawLowVal;
      encoderValueWrap = temp / measuredAcc;
    }

    // Calculate wrap
    if (prevRawEncoder < (encRawLowVal+100) && rawEncoderValue >= (encRawHighVal-100))
    {
      windCount--;
    }
    else if (prevRawEncoder > (encRawHighVal-100) && rawEncoderValue <= (encRawLowVal+100))
    {
      windCount++;
    }
    encoderValueWind = (windCount*encHighVal) + encoderValueWrap;
    
    prevRawEncoder = rawEncoderValue;
  }
}

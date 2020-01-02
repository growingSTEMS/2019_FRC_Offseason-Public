#include <Wire.h>

//Pin 6 - Servo Signal
//Pin 2 - Encoder Signal In
//Pin A5 - SCL
//Pin A4 - SDA

const int servoAddress = 16;
byte servoPos = 0;

const int arraySize = 255;
const int kMaxElementCount = 255;

uint16_t prevPos[3] = {0, 0, 0};
uint8_t positions[arraySize];

void setup() {
  Wire.begin(15);
  Serial.begin(115200);
}

void printArray(uint8_t arr[], int arrSize)
{
  for(int i=0; i<arrSize; i++)
  {
    if (arr[i] != 0)
    {
      Serial.print(i, DEC);
      Serial.print(",");
      Serial.println(arr[i], DEC);
    }
  }
}

void appendAndDelete(uint16_t arr[], int arrSize, uint16_t newValue)
{
  for (int i=0; i<(arrSize-1); i++)
  {
    arr[i] = arr[i+1];
  }
  arr[arrSize-1] = newValue;
}

bool doesArrayContain( uint16_t arr[], int arrSize, uint16_t checkValue)
{
  for (int i=0; i<arrSize; i++)
  {
    if (arr[i] == checkValue)
    {
      return true;
    }
  }
  return false;
}

void loop()
{
//  int posA = 20;
//  int posB = 240;
//
//  if (servoPos == posA)
//  {
//    servoPos = posB;
//  }
//  else
//  {
//    servoPos = posA;
//  }

  servoPos -= 10;
  if (servoPos == 253)
  {
    servoPos = 0;
  }
  else if (servoPos == 255)
  {
    servoPos = 252;
  }
  
  Wire.beginTransmission(servoAddress);
  Wire.write(servoPos);
  Wire.endTransmission();

  if (Wire.requestFrom(servoAddress, 1) == 1)
  {
    byte byte0 = Wire.read();
    Serial.println(byte0, DEC);
  }

//  servoSpeed += 1;
//  if (servoSpeed > 1720)
//  {
//    servoSpeed = 1280;
//  }
  
//  if (Wire.requestFrom(servoAddress, 1) == 1)
//  {
////    byte byte0 = Wire.read();
////    byte byte1 = Wire.read();
////    uint16_t encoder = word(byte1, byte0);
//      byte encoder = Wire.read();
//
//    if (encoder >= arraySize)
//    {
//      Serial.println("Encoder position bigger than array!");
//    }
//    else if (!doesArrayContain(prevPos, 3, encoder))
//    {
//      positions[encoder]++;
//      if (positions[encoder] >= kMaxElementCount)
//      {
//        printArray(positions, arraySize);
//        Serial.println("-------");
//        Serial.print(encoder, DEC);
//        Serial.print(",");
//        Serial.println(positions[encoder], DEC);
//        while (1);
//      }
//
//      appendAndDelete(prevPos, 3, encoder);
//    }
//    //Serial.println( encoder );
//  }
//  else
//  {
//    Serial.println( "Failed RX" );
//  }
  delay(100);
}

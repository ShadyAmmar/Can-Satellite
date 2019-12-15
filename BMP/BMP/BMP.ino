#include <Wire.h>
#define BMP085_ADDRESS 0x77 
const unsigned char OSS = 0;  // Oversampling Setting

int ac1,ac2,ac3,b1,b2,mb,mc,md;
unsigned int ac4,ac5,ac6;
long b5; 

short temperature;
long pressure;


int ReadInt(unsigned char address)
{  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2);  
  return (int) Wire.read()<<8 | Wire.read();
}


//Stores all of the calibration values
void bmpCalibration()
{
  ac1 = ReadInt(0xAA);
  ac2 = ReadInt(0xAC);
  ac3 = ReadInt(0xAE);
  ac4 = ReadInt(0xB0);
  ac5 = ReadInt(0xB2);
  ac6 = ReadInt(0xB4);
  b1 = ReadInt(0xB6);
  b2 = ReadInt(0xB8);
  mb = ReadInt(0xBA);
  mc = ReadInt(0xBC);
  md = ReadInt(0xBE);
}
//Read the uncompensated temperature value
unsigned int ReadUT()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4 ==== control Reg
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  delay(5);
  //Read the uncompensated temperature valu
  ut = ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long ReadUP()
{
  unsigned long up = 0;
  
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  delay(2 + (3<<OSS));
 
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  while(Wire.available() < 3);  
  up = (((unsigned long) Wire.read() << 16) | ((unsigned long) Wire.read() << 8) | (unsigned long) Wire.read()) >> (8-OSS);
  
  return up;
}

// Temp. in units of 0.1 deg C
short GetTemperature(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

//pressure in units of Pa.
long GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  bmpCalibration();
}
void loop()
{
  temperature = GetTemperature(ReadUT());
  pressure = GetPressure(ReadUP());
  
  Serial.print("Temperature: ");
  Serial.print(temperature*0.1, DEC);
  Serial.println(" deg C");
  Serial.print("Pressure: ");
  Serial.print(pressure, DEC);
  Serial.println(" Pa");
  Serial.println();
}






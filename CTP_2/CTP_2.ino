#include <Wire.h>
#include <SD.h>

///////////////////BMP//////////////////////////
#define BMP085_ADDRESS 0x77 
/////////////////////////////////MPU////////////////////////////////////////////
#define MPU 0x68
#define power_reg_Add 107 
#define acc_x_h 0x3B
#define acc_x_l 0x3C
#define acc_y_h 0x3D
#define acc_y_l 0x3E
#define acc_z_h 0x3F
#define acc_z_l 0x40
#define temp_h 0x41
#define temp_l 0x42
#define gyro_x_h 0x43
#define gyro_x_L 0x44
#define gyro_y_h 0x45
#define gyro_y_l 0x46
#define gyro_z_h 0x47
#define gyro_Z_l 0x48
///////////////////BMP//////////////////////////
const unsigned char OSS = 0;  // Oversampling Setting

int ac1,ac2,ac3,b1,b2,mb,mc,md;
unsigned int ac4,ac5,ac6;
long b5; 

short temperature;
long pressure;


void ReadInt(int *p,unsigned int *p2,int sort,unsigned char address)
{
  Wire.begin();  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2); 
 if(sort ==1) *p = (int) Wire.read()<<8 | Wire.read();
 if(sort ==2) *p2 = (int) Wire.read()<<8 | Wire.read();
}


//Stores all of the calibration values
void bmpCalibration()
{
  ReadInt(&ac1,0,1,0xAA); //ac1
  ReadInt(&ac2,0,1,0xAC); //ac2
  ReadInt(&ac3,0,1,0xAE); //ac3
  ReadInt(0,&ac4,2,0xB0); //ac4
  ReadInt(0,&ac5,2,0xB2); //ac5
  ReadInt(0,&ac6,2,0xB4); //ac6
  ReadInt(&b1,0,1,0xB6);  //b1
  ReadInt(&b2,0,1,0xB8);  //b2
  ReadInt(&mb,0,1,0xBA);  //mb
  ReadInt(&mc,0,1,0xBC);  //mc
  ReadInt(&md,0,1,0xBE);  //md
}

void GetTemperature(short *ptemp)
{
  long x1, x2;
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4 ==== control Reg
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  delay(5);
  //Read the uncompensated temperature valu
  ReadInt(0,&ut,2,0xF6);
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  *ptemp = ((b5 + 8)>>4);  
}

//pressure in units of Pa.
void GetPressure(long *ppress)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
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
    *ppress = (b7<<1)/b4;
  else
    *ppress = (b7/b4)<<1;
    
  x1 = (*ppress>>8) * (*ppress>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * *ppress)>>16;
  *ppress += (x1 + x2 + 3791)>>4;

}
//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////MPU/////////////////
//float acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,temp;
float read_val[7];
//String read_name[7] = {"ACCELERATION_X= ","ACCELERATION_Y= ","ACCELERATION_Z= ","TEMPERATURE= ","GYROSCOPE_X= ","GYROSCOPE_Y= ","GYROSCOPE_Z= "};
void IMU_power_setting(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(power_reg_Add);
  Wire.write(0);
  Wire.endTransmission(); 
}
void IMU(){
    Wire.beginTransmission(MPU);
    Wire.write(acc_x_h);
    Wire.endTransmission();
    Wire.requestFrom(MPU,14);
    if(Wire.available()==14){
      for(int i=0;i<7;i++){
      read_val[i] = (Wire.read()<<8) | Wire.read();
      }
    for(int z=0; z<7;z++){
      //Serial.print(read_name[z]);
      Serial.print(read_val[z]);
      Serial.print("\t");
    }
    Serial.print("\n");
      
    }else{ Serial.println("IMU data not available"); }

}
/////////////////////////////////////////GPS//////////////////////////////
int serial;
char Massege_id[6];
char UTC[11];
char Latitude[10];
char NS_indicator[2];
char Longitude[11];
char EW_Indicator[2];
char Position_Fix_Indicator[2];
char Satellites_Used[3];
char MSL_Altitude[7];
char Status[2];
char Speed[4];
char Random[20];
  
  void Print(String name,char Array[],int len){
   Serial.print(name);
   Serial.print(" : ");
    for(int i=0;i<len;i++){
            Serial.print(Array[i]);
            
   }
   Serial.write("\n");
  
  }
void GPS(){
    while(Serial.read() == '$'){
      Serial.readBytesUntil(',',Massege_id,'7');
      if(Massege_id[2]=='G' && Massege_id[3]=='G' && Massege_id[4]=='A'){
        Serial.readBytesUntil(',',UTC,'12');
        Serial.readBytesUntil(',',Latitude,'11');
        Serial.readBytesUntil(',',NS_indicator,'3');
        Serial.readBytesUntil(',',Longitude,'12');
        Serial.readBytesUntil(',',EW_Indicator,'3');
        Serial.readBytesUntil(',',Position_Fix_Indicator,'3');
        Serial.readBytesUntil(',',Satellites_Used,'4');
        Serial.readBytesUntil(',',Random,'5');
        Serial.readBytesUntil(',',MSL_Altitude,'8');
        Serial.println("-------------------------------------------------------");
        Serial.println("GPGGA :");
        Print("UTC position",UTC,11);
        Print("Latitude",Latitude,10);
        Print("NS_indicator",NS_indicator,2);
        Print("Longitude",Longitude,11);
        Print("EW_Indicator",EW_Indicator,2);
        Print("Position_Fix_Indicator",Position_Fix_Indicator,2);
        Print("Satellites_Used",Satellites_Used,3);
        Print("MSL_Altitude",MSL_Altitude,7);
        
      
      }
      else if(Massege_id[2]=='R' && Massege_id[3]=='M' && Massege_id[4]=='C'){
        Serial.readBytesUntil(',',Random,'12');
        Serial.readBytesUntil(',',Status,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Speed,'5');   
        Serial.println("GPRMC :");
        Print("Status",Status,2);
        Print("Speed",Speed,4); 
        Serial.println("-------------------------------------------------------");     
        IMU();
        GetTemperature(&temperature);
        GetPressure(&pressure);
  
      Serial.print("Temperature: ");
      Serial.print(temperature*0.1, DEC);
      Serial.println(" deg C");
      Serial.print("Pressure: ");
      Serial.print(pressure, DEC);
      Serial.println(" Pa");
      Serial.println("-------------------------------------------------------");
      }
      else Serial.println("invalid MsgID");
    
    }
  
}
////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  //////////////////////MPU////////////////
  IMU_power_setting();
  ////////////////////////BMP//////////////
 
  bmpCalibration();
  //////////////////////////////////////////
  pinMode(3,OUTPUT);
  if(!SD.begin(3)) Serial.println("Card Failed");
 else Serial.println("Card initialized"); 
}

void loop()
{


    while(Serial.available()>0){
    GPS();
    }
    Serial.println("No data in Serial from GPS");
  File dataFile = SD.open("CTP.txt",FILE_WRITE);
     if(dataFile) {
    dataFile.println("Shady CTP");
    dataFile.close();
    Serial.println("Done");
  }
  else Serial.println("Error opening CTP.txt"); 
}

#include <Wire.h>
#include <SD.h>
//#include <idDHT11.h>
float x=0;
float y=0;
int i=0;
int puzzer=0;
char old_Altitude[7];
File dataFile;
//////////////////////DHT///////////////////////
/*int idDHT11pin = 2; //Digital pin for comunications
int idDHT11intNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dht11_wrapper(); // must be declared before the lib initialization

// Lib instantiate
//idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);*/
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
///////////////////////////DHT/////////////////
/*void dht11_wrapper() {
  DHT11.isrCallback();
}*/
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
  *ptemp = *ptemp *0.1;
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
float IMU_val[10];
//"ACCELERATION_X= ","ACCELERATION_Y= ","ACCELERATION_Z= ","TEMPERATURE= ","GYROSCOPE_X= ","GYROSCOPE_Y= ","GYROSCOPE_Z= ","th_x","th_y","th_z"};
void IMU_power_setting(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(power_reg_Add);
  Wire.write(0);
  Wire.endTransmission(); 
}
void IMU(){
  int theta_x;
  int theta_y;
  int theta_z;
    Wire.beginTransmission(MPU);
    Wire.write(acc_x_h);
    Wire.endTransmission();
    Wire.requestFrom(MPU,14);
    if(Wire.available()==14){
      for(int i=0;i<7;i++){
      IMU_val[i] = (Wire.read()<<8) | Wire.read();
      }
      IMU_val[0]=IMU_val[0]*9.8/16100;        //multiplying by gravity acceleration and dividing by sensitivity to get the real value of the acceleration,data sheet(1),page(13)
      IMU_val[1]=IMU_val[1]*9.8/16100;
      IMU_val[2]=IMU_val[2]*9.8/16100;
      IMU_val[3]=IMU_val[3]/340+36.53;             //dividing by sensitivity t get real value of temperature,data sheet(1),page(14)
      IMU_val[4]=IMU_val[4]/131;                   //dividing by sensitivity t get real value of gyroscope,data sheet(1),page(12)
      IMU_val[5]=IMU_val[5]/131;
      IMU_val[6]=IMU_val[6]/131;
      theta_x = (IMU_val[4]*y/1000);
      theta_y = (IMU_val[5]*y/1000);
      theta_z = (IMU_val[6]*y/1000);
      IMU_val[7]=IMU_val[7]+theta_x;
      IMU_val[8]=IMU_val[8]+theta_y;
      IMU_val[9]=IMU_val[9]+theta_z;
}
}
/////////////////////////////////////////GPS//////////////////////////////
int serial;
char Massege_id[6];
char UTC[11];
char MSL_Altitude[7];
char Latitude[10];
char NS_indicator[2];
char Longitude[11];
char EW_Indicator[2];
char Satellites_Used[3];
char Status[2];
char Random[20];
char Speed[4];

  void Print(int sort,char Array[],int len){
   if(sort ==0){
      for(int i=0;i<len;i++){
              Serial.print(Array[i]);
      }
      Serial.print("\n");
      
   }
   if(sort ==1){
      for(int i=0;i<len;i++){
              dataFile.print(Array[i]);
      }
      dataFile.print(","); 
   }
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
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Satellites_Used,'4');
        //Serial.readBytesUntil(',',Random,'5');
        //Serial.readBytesUntil(',',MSL_Altitude,'8');
        
      
      }/*else if(Massege_id[2]=='R' && Massege_id[3]=='M' && Massege_id[4]=='C'){
        Serial.readBytesUntil(',',Random,'12');
        Serial.readBytesUntil(',',Status,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Speed,'5');    
      
      }*/
    
    }
  
}
////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  //////////////////////MPU////////////////
  IMU_power_setting();
  IMU_val[7] = 0;
  IMU_val[8] = 0;
  IMU_val[9] = 0;
  ////////////////////////BMP//////////////
 
  bmpCalibration();
  //////////////////////////////////////////
  pinMode(10,OUTPUT);
  pinMode(7,OUTPUT);
  //pinMode(2,OUTPUT);
  if(!SD.begin(10)) Serial.print("Card Failed");
 else Serial.print("Card initialized"); 
 dataFile = SD.open("Final.csv",FILE_WRITE);
     if(dataFile) {
    dataFile.println("UTC,Latitude,Longitude,N/S,E/W,Satellites,Status,A_x,A_y,A_z,Temp_IMU,G_x,G_y,G_z,th_x,th_y,th_z,Pressure");
   dataFile.close();  
   }
}

void loop()
{
    y = millis()-x;
    x = millis();
    //Serial.println(y);
    //////////////////Old Altitude Store///////////
   /* for(int i=0;i<7;i++){
      old_Altitude[i] = MSL_Altitude[i];
    }
  */
    //initialize GPS
    Serial.print("start");
    while(Serial.available()>0){
    GPS();
    }
    /////////////////////Check Altitude///////////
    //if(old_Altitude[0] == MSL_Altitude[0] && old_Altitude[1] == MSL_Altitude[1] && old_Altitude[2] == MSL_Altitude[2] && old_Altitude[3] == MSL_Altitude[3] && old_Altitude[4] == MSL_Altitude[4] && old_Altitude[5] == MSL_Altitude[5] && old_Altitude[6] == MSL_Altitude[6]) puzzer =1;
    //Sending on RF
    Serial.print("UTC,");
    Print(0,UTC,11);
    Serial.print("Latitude,");
    Print(0,Latitude,10);
    Serial.print("Longitude,");
    Print(0,Longitude,11);
    Serial.print("NS_indicator,");
    Print(0,NS_indicator,2);
    Serial.print("EW_Indicator,");
    Print(0,EW_Indicator,2);
    Serial.print("Satellites_Used,");
    Print(0,Satellites_Used,3);
    Serial.print("Status,");
    Print(0,Status,2);
    
    //initialize IMU
    IMU();
    //Sending on RF
    Serial.print("IMU,");
    for(int z=0; z<10;z++){
      Serial.print(IMU_val[z]);
      Serial.print("//");
    }
    
    //initialize BMP
    GetTemperature(&temperature);
    GetPressure(&pressure);
    //Sending on RF
    // Serial.print("Temperature,");
    // Serial.println(temperature);
     Serial.print("Pressure,");
     Serial.println(pressure);
     
     //initialize DHT
    // DHT11.acquire();
    // while (DHT11.acquiring());
     //int result = DHT11.getStatus();
     //Sending on RF
     //Serial.println(DHT11.getHumidity(), 2);
     
     //End of data Block
     Serial.println("End");
  
  //Storing on SD_Card   
  dataFile = SD.open("Final.csv",FILE_WRITE);
     if(dataFile) {
       
    //GPS/
    Print(1,UTC,11);
    Print(1,Latitude,10);
    Print(1,Longitude,11);
    Print(1,NS_indicator,2);
    Print(1,EW_Indicator,2);
    Print(1,Satellites_Used,3);
    Print(1,Status,2);
    //INU
    for(int z=0; z<10;z++){
      dataFile.print(IMU_val[z]);
      dataFile.print(",");
    }
    //BMP
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.println(pressure);
    
    dataFile.close();
  }else Serial.println("SD_Card Failed");

}

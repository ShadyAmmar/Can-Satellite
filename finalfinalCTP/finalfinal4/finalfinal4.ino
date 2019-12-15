#include <Wire.h>
#include <SD.h>
//#include <idDHT11.h>
#define puzzer_pin 8
int puzzer_var = 1;
int crash=0;
float old = 1058.21;
float newacc =0;
int var =1;
float x=0;
float y=0;
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
float imu_val[10];
float thx=0;
float thy=0;
float thz=0;
//String imu_name[10] = {"ACCELERATION_X= ","ACCELERATION_Y= ","ACCELERATION_Z= ","TEMPERATURE= ","GYROSCOPE_X= ","GYROSCOPE_Y= ","GYROSCOPE_Z= ","th_x= ","th_y= ","th_z= "};
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
      imu_val[i] = (Wire.read()<<8) | Wire.read();
      }
    Wire.endTransmission(); 
    imu_val[0]=imu_val[0]*9.8/16384;
    imu_val[1]=imu_val[1]*9.8/16384;
    imu_val[2]=imu_val[2]*9.8/16384;
    imu_val[3]=imu_val[3]/340+36.53;
    imu_val[4]=imu_val[4]/131;
    imu_val[5]=imu_val[5]/131;
    imu_val[6]=imu_val[6]/131;
    theta_x = (imu_val[4]*y/1000);
    theta_y = (imu_val[5]*y/1000);
    theta_z = (imu_val[6]*y/1000);
    thx=thx+theta_x;
    thy=thy+theta_y;
    thz=thz+theta_z;
    imu_val[7] = thx;
    imu_val[8] = thy;
    imu_val[9] = thz;      
    }

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

  void Print(int sort,char Array[],int len){
   if(sort ==0){
      for(int i=0;i<len;i++){
              Serial.print(Array[i]);
      }
      Serial.write("\n");
      
   }
   if(sort ==1){
      for(int i=0;i<len;i++){
              dataFile.print(Array[i]);
      }
      dataFile.print(","); 
   }
  }
void GPS(){
  while(Serial.available()>0){
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
      
      }
      else if(Massege_id[2]=='R' && Massege_id[3]=='M' && Massege_id[4]=='C'){
        Serial.readBytesUntil(',',Random,'12');
        Serial.readBytesUntil(',',Status,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Speed,'5');    
      
      }
    }
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
  pinMode(10,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(puzzer_pin,OUTPUT);
  if(!SD.begin(10)) Serial.print("Card Failed");
 else Serial.print("Card initialized"); 
 dataFile = SD.open("DeadSat.csv",FILE_WRITE);
     if(dataFile) {
    dataFile.println("UTC,Latitude,NS_indicator,Longitude,EW_Indicator,Position_Fix_Indicator,Satellites_Used,MSL_Altitude,Status,Speed,A_x,A_y,A_z,Temp_IMU,G_x,G_y,G_z,th_x,th_y,th_z,Pressure");
   dataFile.close();  
   }
}

void loop()
{
    y = millis()-x;
    x = millis();
    
    //initialize GPS
    Serial.println("start"); 
    GPS();
    
    
    //Sending on RF 
    
        Serial.print("dsutc");
        Print(0,UTC,11);
        Serial.print("dslat");
        Print(0,Latitude,10);
        Serial.print("dsns");
        Print(0,NS_indicator,2);
         Serial.print("dslong");
        Print(0,Longitude,11);
        Serial.print("dsew");
        Print(0,EW_Indicator,2);
        Serial.print("dspos");
        Print(0,Position_Fix_Indicator,2);
        Serial.print("dssat");
        Print(0,Satellites_Used,3);
        Serial.print("dsmsl");
        Print(0,MSL_Altitude,7);
    Serial.print("dssta");
        Print(0,Status,2);
        Serial.print("dsspe");
        Print(0,Speed,4); 
    
    //initialize IMU
    IMU();
    
    //Sending on RF
    
      Serial.print("dsax");
      Serial.println(imu_val[0]);
      Serial.print("dsay");
      Serial.println(imu_val[1]);
      Serial.print("dsaz");
      Serial.println(imu_val[2]);
      Serial.print("dstem");
      Serial.println(imu_val[3]);
      Serial.print("dsgx");
      Serial.println(imu_val[4]);
      Serial.print("dsgy");
      Serial.println(imu_val[5]);
      Serial.print("dsgz");
      Serial.println(imu_val[6]);
      Serial.print("dstx");
      Serial.println(imu_val[7]);
      Serial.print("dsty");
      Serial.println(imu_val[8]);
      Serial.print("dstz");
      Serial.println(imu_val[9]);
    
    
    //initialize BMP
    GetTemperature(&temperature);
    GetPressure(&pressure);
    //Sending on RF
    // Serial.print("Temperature,");
    // Serial.println(temperature);
     Serial.print("dspre");
     Serial.println(pressure);
  
  //Storing on SD_Card   
  dataFile = SD.open("DeadSat.csv",FILE_WRITE);
     if(dataFile) {
       
    //GPS/
        Print(1,UTC,11);
        Print(1,Latitude,10);
        Print(1,NS_indicator,2);
        Print(1,Longitude,11);
        Print(1,EW_Indicator,2);
        Print(1,Position_Fix_Indicator,2);
        Print(1,Satellites_Used,3);
        Print(1,MSL_Altitude,7);
        Print(1,Status,2);
        Print(1,Speed,4); 
    //INU
    for(int z=0; z<10;z++){
      dataFile.print(imu_val[z]);
      dataFile.print(",");
    }
    //BMP
    //dataFile.print(temperature);
    //dataFile.print(",");
    dataFile.println(pressure);
    dataFile.close();
  }else Serial.println("SD_Card Failed");
  
  //puzzer
  puzzer();

}
void puzzer(){
  newacc = imu_val[1]*imu_val[1]+imu_val[2]*imu_val[2]+imu_val[3]*imu_val[3];
  Serial.println("-----------------");
  Serial.println(newacc);
  Serial.println(old);
  Serial.println(newacc-old);
  Serial.println("-----------------");
  if(newacc-old>100){
    crash = 1;
  }
  if(crash==0){
  puzzer_var ^= 1;
  digitalWrite(puzzer_pin,puzzer_var);
  }else{
   digitalWrite(puzzer_pin,HIGH);
  }
  old = newacc;
}


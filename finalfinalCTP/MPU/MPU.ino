#include <Wire.h>
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
//float acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,temp;

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
      
      }
      else Serial.println("invalid MsgID");
    
    
    }
    } Serial.println("No data");
   
  
}

int puzzer_pin =8;
float imu_val[10];
float y=0;
float x=0;
float thx=0;
float thy=0;
float thz=0;
String imu_name[10] = {"ACCELERATION_X= ","ACCELERATION_Y= ","ACCELERATION_Z= ","TEMPERATURE= ","GYROSCOPE_X= ","GYROSCOPE_Y= ","GYROSCOPE_Z= ","th_x= ","th_y= ","th_z= "};
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
    imu_val[8] = thx;
    imu_val[9] = thx;
    
    for(int z=0; z<10;z++){
      Serial.print(imu_name[z]);
      Serial.print(imu_val[z]);
      Serial.print("\t");
    }
    Serial.print("\n");
      
    }else{ Serial.println("IMU data not available"); }

}

void puzzer(){
 if((imu_val[0])*(imu_val[0]) + (imu_val[1])*(imu_val[1]) + (imu_val[2])*(imu_val[2]) > 120){
   digitalWrite(puzzer_pin, HIGH);
 }else{
   digitalWrite(puzzer_pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(puzzer_pin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
 }
}
void setup()
{
  Serial.begin(9600);
  pinMode(puzzer_pin, OUTPUT); 
  IMU_power_setting();  
 
}

void loop()
{
    y = millis()-x;
    x = millis();
    //IMU();
    //puzzer();
     GPS();
    //delay(500);
  
}

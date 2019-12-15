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
float read_val[7];
String read_name[7] = {"ACCELERATION_X= ","ACCELERATION_Y= ","ACCELERATION_Z= ","TEMPERATURE= ","GYROSCOPE_X= ","GYROSCOPE_Y= ","GYROSCOPE_Z= "};
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
      Serial.print(read_name[z]);
      Serial.print(read_val[z]);
      Serial.print("\t");
    }
    Serial.print("\n");
      
    }else{ Serial.println("IMU data not available"); }

}

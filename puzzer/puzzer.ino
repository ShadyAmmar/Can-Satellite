
char Massege_id[6];
char UTC[11];
char Latitude[10];
char NS_indicator[2];
char Longitude[11];
char EW_Indicator[2];
char Position_Fix_Indicator[2];
char Satellites_Used[3];
char MSL_Altitude[7];
char old_Altitude[7];
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
/*void puzzer(int x){
 if(x==0){
 analogWrite(2,150);
 }
 if(x==1){
 analogWrite(2,255);
 }
  
}*/
int GPS(){
    int x=0;
    for(int i=0;i<=7;i++){
      old_Altitude[i]=MSL_Altitude[i];
    }
    if(Serial.read() == '$'){
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
        Print("Longitude",Longitude,11);
        Print("NS_indicator",NS_indicator,2);
        Print("EW_Indicator",EW_Indicator,2);
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
    
    
    }
    if(Status[0]=='A'){
    for(int i=0;i<6;i++){
      if(old_Altitude[i] != MSL_Altitude[i]) i=7;
      if(i==5) x=1; 
    }
    }
    return x;   
  
}
int timer=0;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  
}

void loop()
{
  
  int state=GPS();
  if(state==0){
 if(timer>20){
    analogWrite(2,150);
 }
 if(timer>30){
   analogWrite(2,0);
   timer =0;
 }
 }
 if(state==1){
 analogWrite(2,255);
 }
 timer = timer+1;
}




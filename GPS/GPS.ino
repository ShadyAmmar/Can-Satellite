
/*char Massege_id[6];
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
*/

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

/*void Print(String name,char Array[],int len){
 Serial.print(name);
 Serial.print(" : ");
  for(int i=0;i<len;i++){
          Serial.write(Array[i]);
          
 }
 Serial.write("\n");

}*/

void Print(int sort,char Array[],int len){
   if(sort ==0){
      for(int i=0;i<len;i++){
              Serial.print(Array[i]);
      }
      Serial.write("\n");
      
   }
   /*if(sort ==1){
      for(int i=0;i<len;i++){
              dataFile.print(Array[i]);
      }
      dataFile.print(","); 
   }*/
  }

/*void GPS(){
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
        Serial.readBytesUntil(',',Random,'11');
        Serial.readBytesUntil(',',Status,'2');
        Serial.readBytesUntil(',',Random,"11");
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Random,"11");
        Serial.readBytesUntil(',',Random,'3');
        Serial.readBytesUntil(',',Speed,'5');   
        Serial.println("GPRMC :");
        if(Status[0] != 'V') Status[0] = 'A';
        Print("Status",Status,1);
        Print("Speed",Speed,5); 
        Serial.println("-------------------------------------------------------");     
      
      }
      else {}//Serial.println("invalid MsgID");
    
    
    }
    } 
    //Serial.println("No data");   
}*/

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

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  
}

void loop()
{
  //int x =0;
  //int y =0;
 // x = millis();
 //GPS();

/*Sending on RF 
    
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
        Print(0,Speed,4); */
        
 // y = millis()-x;
  //Serial.println("Time is");
 // Serial.println(y);
 Serial.write(Serial.read());
}

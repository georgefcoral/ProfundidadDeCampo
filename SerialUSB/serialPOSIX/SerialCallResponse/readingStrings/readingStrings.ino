
int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte = 0;         // incoming serial byte
char reading;
char reading2;
int readValue = 0;
size_t numBytes = 10; 
char inChar[3]={'s','b','f'};
byte buff;
byte buffO;
String seq = "";
void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(115200 );
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}


double measures[5] = {123.123,94.2,552.50,60.1,0.23};//Simulación de las mediciones en mm.


//This function returns a total step received and the flag for forward, backward a stop.
int deCode (String buff,int &flag){
  if( buff.charAt(0)== '0'){
    flag = 0;
    return 0;
  }
  if(buff.charAt(0) == '1'){
    flag = -1;
    return buff.toInt();
  }
  if(buff.charAt(0) == '2'){
    flag = 1;
    return buff.toInt();
  }
  return -1;
}
int step1;
int flag = 0;
String medTemp = "";
bool ad;

int k=0;
void loop()
{
  
  if (Serial.available())
  {
    do{
      buff = Serial.read();
      if(buff == 'a')
        break;
      ad = seq.concat(char(buff));
      //Serial.write(seq.charAt(k));
      //k++;
    }while(1);
  
  step1 = deCode(seq,flag);
  int j = 0;
    medTemp = String(measures[k]);
    do{  
      buffO = medTemp[j];
      if(j== String(measures[k]).length()-1){
        break;
      }
      Serial.write(buffO);
      j++;
    }while(1);
    
//    if(seq !=""){
//    step1 = deCode(seq,flag);
//    if(flag == 0){
//      Serial.write(inChar[0]);
//    }
//    if(flag == -1){
//      Serial.write(inChar[1]);
//    }
//    if(flag == 1){
//       Serial.write(inChar[2]);
//      }
//    }
    seq = "";
    medTemp = "";
    k++;
  }
}

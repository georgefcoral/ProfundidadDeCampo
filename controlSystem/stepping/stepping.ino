
//Librerías necesarias.
//Adafruit_VL53L0X 
//Adafruit_Motor_Shield_library
//AccelStepper 
//AccelStepperWithDistances


#include "Adafruit_VL53L0X.h"
#include <AFMotor.h>



// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(200, 2);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();




void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(115200 );
  motor.setSpeed(100);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  
  //motor.setSpeed(100);  // 10 rpm  
  lox.begin();
  
//  if (!lox.begin()) {
//    Serial.println(F("Failed to boot VL53L0X"));
//  }

}

//This function returns a total step received and the flag for forward, backward a stop.

int deCodeStepping(String str){
  AF_Stepper motor(200, 2);
  int signo = 0;
  int stepping;
  String numString;
  
  //Se define si es un número positivo o negativo.
  if(str[0]== '-'){
    signo = -1;
    numString = str.substring(1,str.length());
  }else{
    signo = 1;
    numString = str;
  }

  stepping = signo*(numString.toInt()); 

  return stepping;
}






int stepping;
int flag = 0;
String medTemp = "";
bool ad;
int readyToWrite = 0;
int k=0;
byte buff;
byte buffO;
String seq = "";
String message = "13.21";
String sender;
int sending=-1;
void loop()
{
  VL53L0X_RangingMeasurementData_t measure;
  
  if (Serial.available())
  {
    
    message= Serial.readString(); 
    stepping = deCodeStepping(message);
    
    
    //stepping contains de input string convert to int.
    //steppeing is data obtained by Serial port.

    /*AGREGAR AQUI MOVIMIENTO DEL MOTOR*/
    
    if(stepping > 0){
        motor.setSpeed(100);
        motor.step(abs(stepping), FORWARD, DOUBLE); 
    }else{
      if(stepping < 0){
        motor.setSpeed(100);
        motor.step(abs(stepping), BACKWARD, DOUBLE); 
      }else{
         motor.setSpeed(100);
         motor.step(abs(stepping), BACKWARD, DOUBLE); 
      }
    }
    //sender = String(stepping);
    motor.release();
    delay(3000);
    readyToWrite = 1;
    
    
    
  }else{  
    
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    
    if(readyToWrite == 1){
      
      if (measure.RangeStatus != 4){  // phase failures have incorrect data
        sending = measure.RangeMilliMeter;
      } else {
        sending = -1;
      }
    
      sender = String(sending);
      int j = 0;
      do{
        buffO = sender[j];
        Serial.write(buffO);
        if(j==sender.length()){ 
          readyToWrite = 0;
          break;
        }
      j++;
      }while(true);   
  }

  
 }

    seq = "";
    medTemp = "";
   
}

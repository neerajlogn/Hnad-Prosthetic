#define SAMPLE_RATE 750
#define BAUD_RATE 9600
#define INPUT_PIN A0
#define BUFFER_SIZE 128
#include <Servo.h>
int circular_buffer[BUFFER_SIZE];
int data_index, sum;


int pos1=120;
int pos2=0;
int sensor_value;
int signal;
int envelop;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

int val;
void setup() {
  servo1.attach(3);
  servo2.attach(4);
  servo3.attach(5);
  servo4.attach(6);
  servo5.attach(7);
  pinMode(1,OUTPUT);//high for malfunction 
  pinMode(10,INPUT);
   pinMode(11, INPUT); 
  pinMode(8, OUTPUT);//yellow
  pinMode(9, OUTPUT);//green
  pinMode(12, OUTPUT);//red
  pinMode(13, OUTPUT);//temp
  Serial.begin(BAUD_RATE);
}

void loop() {
  temperature();
  pressure();


 
  // hath ki code hai 
 
  static unsigned long past = 0;
  unsigned long present = micros();
  unsigned long interval = present - past;
  past = present;
  static long timer = 0;
  timer -= interval;
  if(timer < 0) {
    timer += 1000 / SAMPLE_RATE;
     sensor_value = analogRead(INPUT_PIN);
     signal = EMGFilter(sensor_value);
     envelop = getEnvelop(abs(signal));
   
    if(envelop >150){ //malfunction
      digitalWrite(1,HIGH);
      digitalWrite(2,HIGH);
      delay(200);
      digitalWrite(2,LOW);
    }
    else{
      digitalWrite(1,LOW);
      digitalWrite(2,LOW);
    }

     
    //Serial.println(envelop);
   
     if (envelop > 15){
        servo1.write(pos2);
        servo2.write(pos2);
        servo3.write(pos2);
        servo4.write(pos2);
        servo5.write(pos2);
       
     }
   else if (envelop < 15){
   
      servo1.write(pos1);
      servo2.write(pos1);
      servo3.write(pos1);
      servo4.write(pos1);
      servo5.write(pos1);
     
    }
  }
 
}


  void temperature(){
  int rawValue = analogRead(A1);
  float voltage = rawValue * (5.0 / 1023.0); 
  float resistance = (5.0 * 10000.0) / voltage - 10000.0;
  float temperature = 1.0 / ((1.0 / 298.15) + (1.0 / 10000.0) * log(resistance / 10000.0)); 
  temperature -= 273.15; 
                                 
  if(temperature > 35){
      digitalWrite(13, HIGH);  
  }
  else {
    digitalWrite(13, LOW);
    }
  }




 void pressure(){
   int e = envelop ;
  if(e>15 && e<20) {
      digitalWrite(8, HIGH);
    }
    else if (e> 20 && e<30){
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      }
      else if(e>30){
        digitalWrite(8, HIGH);
        digitalWrite(9, HIGH);
        digitalWrite(12, HIGH);
      }
       else{
          digitalWrite(8, LOW);
        digitalWrite(9, LOW);
        digitalWrite(12, LOW);
        }
 }

 
void fingerr() {

  int sensor2 = analogRead(A2);
  int sensor3 = analogRead(A3);
  int sensor4 = analogRead(A4);
  int sensor5 = analogRead(A5);

  // Check each sensor value and control corresponding servo motor
  if (sensor2 >= 300 && sensor2 <= 1200) {
    servo1.write(120);
    servo2.write(120);
    servo3.write(120);
    servo4.write(120);
    servo5.write(120);
    Serial.println(sensor2);
  }
  else
  
  if (sensor3 >= 300 && sensor3 <= 1200) {
    servo1.write(0);
    servo2.write(0);
    servo3.write(0);
    servo4.write(0);
    servo5.write(0);
    //Serial.println("sensor3");
  }

  if (sensor4 >= 300 && sensor4 <= 1200) {
    servo1.write(120);
    servo2.write(120);
    servo3.write(120);
    servo4.write(120);
    servo5.write(100);
    //Serial.println("sensor4");
  }

  if (sensor5 >= 300 && sensor5 <= 1200) {
    servo1.write(120);
    servo2.write(120);
    servo3.write(120);
   // Serial.println("sensor5");
  }

  delay(80); // Adjust delay as needed
}

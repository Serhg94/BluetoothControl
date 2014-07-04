#include <Servo.h>

Servo servo10; 
String s;
//int servo_corr = -5;
//boolean light=false;
//boolean forw=true;
char com[5];
void setup() {    
    
  Serial.begin(9600);
  //Serial.println("System started!");
  //Serial.println("Init 3 OUTPUT");
  TCCR2B = TCCR2B & 0b11111000 | 7;
  //TCCR0B = TCCR0B & 0b11111000 | 4;
  pinMode(12, OUTPUT);  
  //pinMode(13, OUTPUT); 
  //digitalWrite(13, LOW); 
  servo10.attach(10, 620, 2400);
  servo10.write(84);
  //delay(1000);
  //Serial.println("Servo configured on pin 10");
}

void loop()
{
  if(Serial.available() > 0)
  {
    for(int i=0; i<5; i++)
      com[i]='\0';
    Serial.readBytesUntil('/', com, 5);
    s= String(com);
    //Serial.println(s);
    int val = s.toInt();
   // if ((val>69)&&(val<=108))
   //if ((val>60)&&(val<=115))
   if ((val>68)&&(val<112))
        //servo10.write(val+servo_corr);
        servo10.write(val-5);
    else
    if ((val>=500)&&(val<=755))
    {
        //if(!forw)
        //{
        //    analogWrite(3, 0);
        //    delay(50);
        //    forw=true;
        //}
        digitalWrite(12, LOW);
        analogWrite(3, val-500);
    }
    else 
    if ((val>=245)&&(val<500))
    {
        //if(forw)
        //{
        //    analogWrite(3, 0);
        //    delay(50);
        //    forw=false;
        //}
        digitalWrite(12, HIGH);
        analogWrite(3, (-1)*(val-500));
    }
//    if ((val==1))
//    {
//        if(light)
//        {
//          digitalWrite(13, LOW);
//          light=false;
//        }
//        else
//        {
//          digitalWrite(13, HIGH);
//          light=true;
//        }
//    }
  }
  //Serial.flush();
}



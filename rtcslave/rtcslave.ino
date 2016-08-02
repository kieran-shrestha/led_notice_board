#include <EEPROM.h>

#include <Wire.h>
#include "RTClib.h"

#define MSGLIMIT 150

#define TEMPSENSOR A0

#define M1L1 20
#define M2L1 M1L1 + MSGLIMIT+1
#define M3L1 M2L1 + MSGLIMIT+1

#define M1L2 M3L1 + MSGLIMIT+1
#define M2L2 M1L2 + MSGLIMIT+1
#define M3L2 M2L2 + MSGLIMIT+1

#define ADDM1L1 3
#define ADDM2L1 4
#define ADDM3L1 5

#define ADDM1L2 6
#define ADDM2L2 7
#define ADDM3L2 8
//time for which message remains
#define TADDM1L1 13
#define TADDM2L1 14
#define TADDM3L1 15

#define TADDM1L2 16
#define TADDM2L2 17
#define TADDM3L2 18



RTC_DS1307 rtc;

unsigned char msg1Line1size=0,msg2Line1size=0,msg3Line1size=0;
unsigned char msg1Line2size = 0,msg2Line2size=0,msg3Line2size=0;

unsigned char msg1Line1time=0,msg2Line1time=0,msg3Line1time=0;
unsigned char msg1Line2time = 0,msg2Line2time=0,msg3Line2time=0;

String txtMsg = "";

unsigned int temparr[50];

void sendTemp(){
  static char count = 0;
  unsigned int temp=25;
  temp = int( (110.0/1024)*analogRead(TEMPSENSOR));
  temparr[count] = temp;
  count++;
  if(count == 49){
    count = 0;
    for(int i = 0;i<50;i++){
        temp += temparr[i];
    }
    temp = int(temp/50.0);
    Serial.println();
    Serial.print('t');
    Serial.print(temp/10);
    Serial.println(temp%10); 
    Serial.println();
  } 
}


void sendTime(){
    DateTime now = rtc.now();
    int hrs=01,mins=1,secs=1;
    hrs = now.hour();
    mins = now.minute();
    secs = now.second();
    Serial.print('c');
    if(hrs<10)
      Serial.print(0);
    Serial.print(hrs);
    Serial.print(":");
    if(mins<10)
      Serial.print(0);
    Serial.print(mins);
    Serial.print(":");
    if(secs<10)
      Serial.print(0);
    Serial.println(secs);
}



 void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Wire.begin();
  analogReference(INTERNAL);
  rtc.begin();
  txtMsg.reserve(160);
  
  if(EEPROM.read(2)!='c'){
    Serial.println("Initializing..");
    for(int i=20;i<M3L2+150;i++){
    EEPROM.write(i,'x');
    }
    EEPROM.write(ADDM1L1,10);
    EEPROM.write(ADDM2L1,10);
    EEPROM.write(ADDM3L1,10);
    
    EEPROM.write(ADDM1L2,10);
    EEPROM.write(ADDM2L2,10);
    EEPROM.write(ADDM3L2,10);
    
    EEPROM.write(TADDM1L1,10);
    EEPROM.write(TADDM2L1,10);
    EEPROM.write(TADDM3L1,10);
    
    EEPROM.write(TADDM1L2,10);
    EEPROM.write(TADDM2L2,10);
    EEPROM.write(TADDM3L2,10);
    
    EEPROM.write(2,'c');
  }
  
   msg1Line1time= EEPROM.read(TADDM1L1);
   msg2Line1time= EEPROM.read(TADDM2L1);
   msg3Line1time= EEPROM.read(TADDM3L1);

   msg1Line2time = EEPROM.read(TADDM1L2);
   msg2Line2time = EEPROM.read(TADDM2L2);
   msg3Line2time = EEPROM.read(TADDM3L2);
   
  msg1Line1size = EEPROM.read(ADDM1L1);
  msg2Line1size = EEPROM.read(ADDM2L1);
  msg3Line1size = EEPROM.read(ADDM3L1);
  
  msg1Line2size = EEPROM.read(ADDM1L2);
  msg2Line2size = EEPROM.read(ADDM2L2);
  msg3Line2size = EEPROM.read(ADDM3L2);
  
  Serial.println(F("Enter message in format MxLyTT"));
  Serial.println(F("x is message maximum of 3"));
  Serial.println(F("y is line maximum of 2"));
  Serial.println(F("TT is time in minute maximum of 99"));
  
  Serial.print('1');
  for(int i = 0;i<msg1Line1size;i++){
    Serial.write(EEPROM.read(M1L1+i));
  }
  Serial.println();
  
  Serial.print('2');
  for(int i = 0;i<msg1Line2size;i++){
    Serial.write(EEPROM.read(M1L2+i));
  }
  Serial.println();
  delay(1000);

}

unsigned long m1L1millis=0,m2L1millis=0,m3L1millis=0;
unsigned long m1L2millis=0,m2L2millis=0,m3L2millis=0;
unsigned long timemillis = 0,tempmillis=0;

int lastMsgSent = 11;
int lastMsgSent2 = 12;
unsigned long timer = 0;

void loop() {
   if(millis() - timemillis > 5000){
     Serial.println();
     sendTime();
     Serial.println();
     timemillis = millis();
   }
   if(millis() - tempmillis > 250){
     sendTemp();
     tempmillis = millis();
   }
  
   if(lastMsgSent == 11 ){
        if( millis() - m1L1millis > msg1Line1time*60000){
          lastMsgSent = 21;
          Serial.print('1');
          for(int i = 0;i<msg2Line1size;i++){
              Serial.write(EEPROM.read(M2L1+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m2L1millis = millis();
        }
    }
    
   if(lastMsgSent == 21 ){
        if( millis() - m2L1millis > msg2Line1time*60000){
          lastMsgSent = 31;
          Serial.print('1');
          for(int i = 0;i<msg3Line1size;i++){
              Serial.write(EEPROM.read(M3L1+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m3L1millis = millis();
        }
    }
    
   if(lastMsgSent == 31 ){
        if( millis() - m3L1millis > msg3Line1time*60000){
          lastMsgSent = 11;
          Serial.print('1');
          for(int i = 0;i<msg1Line1size;i++){
              Serial.write(EEPROM.read(M1L1+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m1L1millis = millis();
        }
    }
    
   if(lastMsgSent2 == 12 ){
        if( millis() - m1L2millis > msg1Line2time*60000){
          lastMsgSent2 = 22;
          Serial.print('2');
          for(int i = 0;i<msg2Line2size;i++){
              Serial.write(EEPROM.read(M2L2+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m2L2millis = millis();
        }
    }
    
   if(lastMsgSent2 == 22 ){
        if( millis() - m2L2millis > msg2Line2time*60000){
          lastMsgSent2 = 32;
          Serial.print('2');
          for(int i = 0;i<msg3Line2size;i++){
              Serial.write(EEPROM.read(M3L2+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m3L2millis = millis();
        }
    }
    
   if(lastMsgSent2 == 32 ){
        if( millis() - m3L2millis > msg3Line2time*60000){
          lastMsgSent2 = 12;
          Serial.print('2');
          for(int i = 0;i<msg1Line2size;i++){
              Serial.write(EEPROM.read(M1L2+i));
          }
          Serial.println();
          Serial.write(0x0A);
          m1L2millis = millis();
        }
    }
}


void serialEvent(){
while (Serial.available()) {
    char inChar = (char)Serial.read();
    txtMsg += inChar;
    if (inChar == '\n') {
      //M1L1TT
      if(txtMsg.charAt(1) == '1' and txtMsg.charAt(3) == '1'){
          msg1Line1size = txtMsg.length()-7;
          Serial.println(msg1Line1size);
          if(msg1Line1size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg1Line1size = MSGLIMIT;
          }   
          for(uint8_t i = 0;i<msg1Line1size;i++){
              EEPROM.write(M1L1+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg1Line1size);
          
          msg1Line1time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM1L1,msg1Line1size);
          EEPROM.write(TADDM1L1,msg1Line1time);
          
          Serial.print(F("Time inverval of "));Serial.print(msg1Line1time);Serial.println(" mins");
     
    }  else if(txtMsg.charAt(1) == '2' and txtMsg.charAt(3) == '1'){
          msg2Line1size = txtMsg.length()-7;
          if(msg2Line1size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg2Line1size = MSGLIMIT;
              Serial.print(F("Time inverval of "));Serial.print(msg2Line1time);Serial.println(" mins");
          }
          for(uint8_t i = 0;i<msg2Line1size;i++){
              EEPROM.write(M2L1+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg2Line1size);
          
          msg2Line1time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM2L1,msg2Line1size);
          EEPROM.write(TADDM2L1,msg2Line1time);
          
          Serial.print(F("Time inverval of "));Serial.print(msg2Line1time);Serial.println(" mins");
          
    } else if(txtMsg.charAt(1) == '3' and txtMsg.charAt(3) == '1'){
          msg3Line1size = txtMsg.length()-7;
          if(msg3Line1size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg3Line1size = MSGLIMIT;
          }
          
          for(uint8_t i = 0;i<msg3Line1size;i++){
              EEPROM.write(M3L1+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg3Line1size);
          
          msg3Line1time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM3L1,msg3Line1size);
          EEPROM.write(TADDM3L1,msg3Line1time);
          
          Serial.print(F("Time inverval of "));Serial.print(msg3Line1time);Serial.println(" mins");
          
      } else if(txtMsg.charAt(1) == '1' and txtMsg.charAt(3) == '2'){
          msg1Line2size = txtMsg.length()-7;
          if(msg1Line2size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg1Line2size = MSGLIMIT;
          }
          for(uint8_t i = 0;i<msg1Line2size;i++){
              EEPROM.write(M1L2+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg1Line2size);
          
          msg1Line2time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM1L2,msg1Line2size);
          EEPROM.write(TADDM1L2,msg1Line2time);
          
          Serial.print(F("Time inverval of "));Serial.print(msg1Line2time);Serial.println(" mins");
          
      } else if(txtMsg.charAt(1) == '2' and txtMsg.charAt(3) == '2'){
          msg2Line2size = txtMsg.length()-7;
          if(msg2Line2size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg2Line2size = MSGLIMIT;
              
          }
          for(uint8_t i = 0;i<msg2Line2size;i++){
              EEPROM.write(M2L2+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg2Line2size);
          
          msg2Line2time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM2L2,msg2Line2size);
          EEPROM.write(TADDM2L2,msg2Line2time);
           
          Serial.print(F("Time inverval of "));Serial.print(msg2Line2time);Serial.println(" mins");
          
      }else if(txtMsg.charAt(1) == '3' and txtMsg.charAt(3) == '2'){
          msg3Line2size = txtMsg.length()-7;
          if(msg3Line2size > MSGLIMIT ){
              Serial.print(F("Message exceeds "));Serial.print(MSGLIMIT);Serial.println(F(" characters, will be truncated.."));
              msg3Line2size = MSGLIMIT;
             
          }
          for(uint8_t i = 0;i<msg3Line2size;i++){
              EEPROM.write(M3L2+i,txtMsg.charAt(i+6));
          }
          Serial.print(F("Message successfully updated of characters: "));
          Serial.println(msg3Line2size);
          
          msg3Line2time = (txtMsg.charAt(4)-48)*10 + (txtMsg.charAt(5)-48);
          EEPROM.write(ADDM3L2,msg3Line2size);
          EEPROM.write(TADDM3L2,msg3Line2time);
          
          Serial.print(F("Time inverval of "));Serial.print(msg3Line2time);Serial.println(" mins");
         
      }else if(txtMsg.charAt(0) == 'm'){
          Serial.print(txtMsg.charAt(0));
          Serial.print(txtMsg.charAt(1));
          Serial.print(txtMsg.charAt(2));
          Serial.println();
          Serial.println(F("Line one speed changed,requires reset"));
         
      }else if(txtMsg.charAt(0) == 'n'){
          Serial.print(txtMsg.charAt(0));
          Serial.print(txtMsg.charAt(1));
          Serial.print(txtMsg.charAt(2));
          Serial.println();         
          Serial.println(F("Line two speed changed,requires reset"));
          
      }else if(txtMsg.charAt(0) == 'f'){
          Serial.print(txtMsg.charAt(0));
          Serial.print(txtMsg.charAt(1));
          Serial.println();      
          Serial.println(F("Font changed,requires reset"));
          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '1' and txtMsg.charAt(2) == '1'){
          Serial.print(F("Message one line one, time:"));Serial.println(EEPROM.read(TADDM1L1));
          uint8_t x = EEPROM.read(ADDM1L1);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M1L1+i));
          }
          Serial.println();          
          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '2' and txtMsg.charAt(2) == '1'){
          Serial.print(F("Message two line one, time:"));Serial.println(EEPROM.read(TADDM2L1));
          uint8_t x = EEPROM.read(ADDM2L1);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M2L1+i));
          }
          Serial.println();      
          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '3' and txtMsg.charAt(2) == '1'){
          Serial.print(F("Message three line one, time:"));Serial.println(EEPROM.read(TADDM3L1));
          uint8_t x = EEPROM.read(ADDM3L1);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M3L1+i));
          }
          Serial.println();          
          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '1' and txtMsg.charAt(2) == '2'){
          Serial.print(F("Message one line two, time:"));Serial.println(EEPROM.read(TADDM1L2));
          uint8_t x = EEPROM.read(ADDM1L2);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M1L2+i));
          }
          Serial.println();          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '2' and txtMsg.charAt(2) == '2'){
          Serial.print(F("Message two line two, time:"));Serial.println(EEPROM.read(TADDM2L2));
          uint8_t x = EEPROM.read(ADDM2L2);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M2L2+i));
          }
          Serial.println();          
      }else if(txtMsg.charAt(0) == 'r' and txtMsg.charAt(1) == '3' and txtMsg.charAt(2) == '2'){
          Serial.print(F("Message three line two, time:"));Serial.println(EEPROM.read(TADDM3L2));
          uint8_t x = EEPROM.read(ADDM3L2);
          for(uint8_t i = 0;i<x;i++){
              Serial.write(EEPROM.read(M3L2+i));
          }
          Serial.println();          
      }else if(txtMsg.charAt(0) == '@' and txtMsg.charAt(3) == '@'){
          Serial.println(F("Time adjusted to"));
          uint8_t hours = (txtMsg.charAt(1)-48)*10+txtMsg.charAt(2)-48;
          uint8_t minutes = (txtMsg.charAt(4)-48)*10+txtMsg.charAt(5)-48;
          Serial.print(hours);
          Serial.print(":");Serial.print(minutes);
          rtc.adjust(DateTime(2016,1,1,hours,minutes,0));
          Serial.println();          
      }

      txtMsg = "";
      }
   }

}

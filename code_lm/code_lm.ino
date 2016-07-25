#define DEBUG 1

/*1 for message 1
2 for message 2
c for time
t for temperature
f for font
m for message 1 update time
n for message 2 shift time */

#include <EEPROM.h>
#include <SPI.h>    
#include <DMD.h> 
      
#include <pt.h>
#include <TimerOne.h>   
#include "Arial_black_16.h"
#include "Arial_Black_16_ISO_8859_1.h"

#define DISPLAYS_ACROSS 4
#define DISPLAYS_DOWN 3

#define MSG1LENADD 5
#define MSG2LENADD 7

#define MSG1START 10
#define MSG2START 200

#define MSG1LIMIT 150
#define MSG2LIMIT 150

#define FONT 3
#define MSG1TIME 1
#define MSG2TIME 2

#define BAUD 9600

#define XMAX  DISPLAYS_ACROSS*32
#define YMAX DISPLAYS_DOWN*16

#define XCEN XMAX/2
#define YCEN YMAX/2

#define HRLEN 7
#define MINLEN 12
#define SECLEN 18

static struct pt pt1,pt2,pt3,pt4,pt5;

uint8_t bannerPos=32,bannerR=1;

uint16_t countClock = 0;

int msg1Xpos = 128,msg2Xpos =128;
unsigned char msg1size=0,msg2size=0;
unsigned char msg1time=10,msg2time=10;

char msg1[MSG1LIMIT];
char msg2[MSG2LIMIT];

long colonBlinkTime = 0;

uint8_t hr = 3;
uint8_t mins = 30;
uint8_t secs = 45;

uint8_t temp = 25;

uint8_t hrChar[2]={1,2},minChar[2]={3,0};
uint8_t tempChar[2]={2,5};

int8_t xsec=18,ysec=0;
int8_t xmin = 12,ymin=0;
int8_t xhr = 7,yhr=0;

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);



void drawClkEdge(){
  dmd.drawCircle( XCEN,YCEN  , 23, GRAPHICS_NORMAL );
  dmd.drawCircle( XCEN,YCEN  , 22, GRAPHICS_NORMAL );
  dmd.drawCircle( XCEN,YCEN  , 21, GRAPHICS_NORMAL );
}

void rotate(float angle,char val){
  int8_t xt;
  angle -= 90;
  angle = angle*0.01745329252;  //converting to degree
  if(val == 's'){
      xt = SECLEN;  //lenght of second
      xsec = xt*cos(angle);//-yt*sin(angle);
      ysec = /*yt*cos(angle)+*/xt*sin(angle);
  }else if (val == 'm'){
     xt = MINLEN; //lenght of minute
     xmin = xt*cos(angle);//-yt*sin(angle);
     ymin = /*yt*cos(angle)+*/xt*sin(angle);
  }else if (val == 'h'){
    xt = HRLEN; //lenght of hour
    xhr = xt*cos(angle);//-yt*sin(angle);
    yhr = /*yt*cos(angle)+*/xt*sin(angle);
  }
}

void drawLine(uint8_t oor,uint8_t thick,char val){
  int8_t xx,yy;
  if(val == 's'){
    xx = xsec;
    yy = ysec;
  }else if(val == 'm'){
    xx = xmin;
    yy = ymin;
  }else if (val == 'h'){
    xx = xhr;
    yy = yhr;   
  }
  if(oor){
    dmd.drawLine(  XCEN,  YCEN , int(xx + XCEN ),int(yy + YCEN) , GRAPHICS_OR );
    if(thick)
    dmd.drawLine(  XCEN+1,  YCEN+1 , int(xx + XCEN +1),int(yy + YCEN+1) , GRAPHICS_OR );
  }else{
   dmd.drawLine(  XCEN,  YCEN , int(xx + XCEN ),int(yy + YCEN) , GRAPHICS_NOR);
   if(thick)
   dmd.drawLine(  XCEN+1,  YCEN+1 , int(xx + XCEN+1 ),int(yy + YCEN+1) , GRAPHICS_NOR);
    }
}

void drawSecond(){
    drawLine(0,0,'s');
    rotate(secs*6,'s');
    drawLine(1,0,'s');
}

void drawMinute(){
    drawLine(0,1,'m');
    rotate(mins*6,'m');
    drawLine(1,1,'m');
}

void drawHour(){
    drawLine(0,1,'h');
    rotate(hr*30,'h');
    drawLine(1,1,'h');
}

void drawClock(){
     drawClkEdge();
     drawSecond();
     drawMinute();
     drawHour();
}

void ScanDMD(){ 
  dmd.scanDisplayBySPI();
}

void printTemp(){
  dmd.drawChar(0,0,' ',GRAPHICS_NORMAL);
  dmd.drawChar(9,0,' ',GRAPHICS_NORMAL);
  dmd.drawChar(0,0,temp/10+48,GRAPHICS_NORMAL);
  dmd.drawChar(9,0,temp%10+48,GRAPHICS_NORMAL);
  dmd.drawChar(21,0,'C',GRAPHICS_NORMAL);
  dmd.drawChar(18,0,'`',GRAPHICS_NORMAL);
}


void printTime(){
  dmd.drawChar(89,0,' ',GRAPHICS_NOR);
  dmd.drawChar(98,0,' ',GRAPHICS_NOR);
  dmd.drawChar(111,0,' ',GRAPHICS_NOR);
  dmd.drawChar(120,0,' ',GRAPHICS_NOR);
    
  dmd.drawChar(89,0,hrChar[0],GRAPHICS_OR);
  dmd.drawChar(98,0,hrChar[1],GRAPHICS_OR);
  dmd.drawChar(111,0,minChar[0],GRAPHICS_OR);
  dmd.drawChar(120,0,minChar[1],GRAPHICS_OR);

}

void printBanner(){
  dmd.drawString(bannerPos,1,"KEC",3,GRAPHICS_NORMAL); 
    if(bannerR == 1 )
      bannerPos++;
    else
      bannerPos--;
      
    if(bannerPos > 55)
      bannerR = 0;
    else if( bannerPos < 32)
      bannerR = 1;
}

void printMsg1(){
  static int i=0;
  dmd.drawString(msg1Xpos,16,msg1,msg1size,GRAPHICS_NORMAL);
  msg1Xpos--;  
  i++;
  if( i > 128 + 12*msg1size){
      msg1Xpos = 128;
      i=0;
    }  
}

void printMsg2(){
  static int j=0;
  dmd.drawString(msg2Xpos,32,msg2,msg2size,GRAPHICS_NORMAL);
  msg2Xpos--;  
  j++;
  if( j > 128 + 12*msg2size){
      msg2Xpos = 128;
      j=0;
    }  
}
  
void dmdInit(){
   Timer1.initialize( 5000 );  
   Timer1.attachInterrupt( ScanDMD );  
   dmd.clearScreen( true );   
}

static int thread1(struct pt *pt,long timeout){
  static long t1=0;
  PT_BEGIN(pt);
  while(1){
    PT_WAIT_UNTIL(pt,(millis()-t1)>timeout);
    printTemp();
    #if(DEBUG == 1)
    Serial.print("Temperature ");
    Serial.println(temp);
    #endif;
    t1=millis();
  }
  PT_END(pt);
}

static int thread2(struct pt *pt,long timeout){
  static long t1=0;
  PT_BEGIN(pt);
  while(1){
    PT_WAIT_UNTIL(pt,(millis()-t1)>timeout);
    printTime();
    #if(DEBUG == 1)
    
    Serial.print(hr);Serial.print(':');Serial.println(mins);
    #endif
    t1=millis();
  }
  PT_END(pt);
}

static int thread3(struct pt *pt,long timeout){
  static long t1=0;
  PT_BEGIN(pt);
  while(1){
    PT_WAIT_UNTIL(pt,(millis()-t1)>timeout);
    printBanner();
    t1=millis();
  }
  PT_END(pt);
}

static int thread4(struct pt *pt,long timeout){
  static long t1=0;
  PT_BEGIN(pt);
  while(1){
    PT_WAIT_UNTIL(pt,(millis()-t1)>timeout);
    printMsg1();
    t1=millis();
  }
  PT_END(pt);
}


static int thread5(struct pt *pt,long timeout){
  static long t1=0;
  PT_BEGIN(pt);
  while(1){
    PT_WAIT_UNTIL(pt,(millis()-t1)>timeout);
    printMsg2();
    t1=millis();
  }
  PT_END(pt);
}

String txtMsg = ""; 

void setup() {
  uint8_t x =0;
  txtMsg.reserve(max(MSG1LIMIT,MSG2LIMIT)+2);
  Serial.begin(BAUD);
  
  /**********CHECKING IF CONTORLLER IS INITIALIZED FIRST**************************/
  
  if(EEPROM.read(4)!='c'){              //to check first start up
    EEPROM.write(MSG1LENADD,10);        // msg length of msg 1
    EEPROM.write(MSG2LENADD,10);        //msg length of msg2
    EEPROM.write(FONT,0);
    EEPROM.write(MSG1TIME,10);
    EEPROM.write(MSG2TIME,10);
    EEPROM.write(4,'c');                //first startup check 
  }
  
  /***********LOADING MESSAGE FROM THE MEMORY*************************************/
  
  msg1size = EEPROM.read(MSG1LENADD);
  msg2size = EEPROM.read(MSG2LENADD);
  msg1time = EEPROM.read(MSG1TIME);
  msg2time = EEPROM.read(MSG2TIME);
  
  #if(DEBUG == 1)
  
  Serial.print("message one size ");Serial.println(msg1size);
  Serial.print("message two size ");Serial.println(msg2size);
  Serial.print("message one delay ");Serial.println(msg1time);
  Serial.print("message two delay ");Serial.println(msg2time);
  Serial.print("chosen font ");Serial.println(EEPROM.read(FONT));
  #endif
  for( x  = 0;x<msg1size;x++){
        msg1[x]=EEPROM.read(MSG1START+x);
    }
  for( x  = 0;x<msg2size;x++){
        msg2[x]=EEPROM.read(MSG2START+x);
    }
  
  dmdInit();
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  PT_INIT(&pt3);
  PT_INIT(&pt4);
  PT_INIT(&pt5);

  if(EEPROM.read(FONT) == 0)
    dmd.selectFont(Arial_Black_16);
  else
    dmd.selectFont(Arial_Black_16_ISO_8859_1);
  
}

void loop() {
  static uint8_t clockColon = 0;
  if(countClock == 300){      //incremented every 1 sec so triggred every 5mins ie 300secs
     dmd.clearScreen(true);
     countClock = 0;
     long entryTime = millis();
     while(1){
         secs++;
         if(secs>59)
            secs = 0;
         drawClock();
         printTemp(); 
         printTime();
         if(clockColon)
           dmd.drawChar(107,0,':',GRAPHICS_OR);
         else 
            dmd.drawChar(107,0,':',GRAPHICS_NOR);
         clockColon = !clockColon;
         delay(1000);
         if(millis()- entryTime > 20000)
          break;
      }
      dmd.clearScreen(true);
   }
   
   thread1(&pt1,10000);    //temp
   thread2(&pt2,10000);    //time
   thread3(&pt3,100);      //banner
   thread4(&pt4,1);       //msg1
   thread5(&pt5,1);      //msg2

/************FUNCTION TO BLINK THE COLON EVERY 1 SEC********************************
*************AND TO INCREASE THE TIME FOR ANALOG CLOCK******************************
*************************************************************************************/

 if(millis() - colonBlinkTime > 1000){
  countClock++;
  clockColon = !clockColon;
  if(clockColon)
   dmd.drawChar(107,0,':',GRAPHICS_OR);
  else 
    dmd.drawChar(107,0,':',GRAPHICS_NOR);
  colonBlinkTime= millis();
 }
}

void serialEvent() {
  Timer1.detachInterrupt();
  uint8_t x;
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    txtMsg += inChar;
    if (inChar == '\n') {
      /*****************SETTING FOR MESSAGE 1******************************/
      if(txtMsg.charAt(0) == '1'){
          msg1size = txtMsg.length()-2;
          if(msg1size > MSG1LIMIT ){
             // Serial.print(F("Message 1 exceeds "));Serial.print(MSG1LIMIT);Serial.println(F(" characters, will be truncated.."));
              msg1size = MSG1LIMIT;
          }
          EEPROM.write(MSG1LENADD,msg1size);
          for( x = 0;x<msg1size;x++)
              EEPROM.write(x+MSG1START,txtMsg.charAt(x+1));
          //Serial.print(F("Message 1 successfully updated of characters: "));
          //Serial.println(msg1size);
          for( x  = 0;x<msg1size;x++){
             msg1[x]=EEPROM.read(MSG1START+x);
          }
        
       }
       /**************************SETTING FOR MESSAGE 2 ******************************/
       else if(txtMsg.charAt(0) == '2'){
          msg2size = txtMsg.length()-2;
          if(msg2size > MSG2LIMIT ){
              //Serial.print(F("Message 2 exceeds "));Serial.print(MSG2LIMIT);Serial.println(F(" characters, will be truncated.."));
              msg2size = MSG2LIMIT;
          }
          EEPROM.write(MSG2LENADD,msg2size);
          for(x = 0;x<msg2size;x++)
              EEPROM.write(x+MSG2START,txtMsg.charAt(x+1));
          //Serial.print(F("Message 2 successfully updated of characters: "));
          //Serial.println(msg2size);
          for( x  = 0;x<msg2size;x++){
             msg2[x]=EEPROM.read(MSG2START+x);
            }
        }
        /*************************SETTING FOR GETTING CLOCK*************************/
        else if(txtMsg.charAt(0) == 'c'){
            hrChar[0] = txtMsg.charAt(1);
            hrChar[1] = txtMsg.charAt(2);
            minChar[0] = txtMsg.charAt(4);
            minChar[1] = txtMsg.charAt(5);
            hr = (hrChar[0]-48)*10 + (hrChar[1]-48);
            mins =( minChar[0]-48)*10 + (minChar[1]-48);
        }
        /*****************setting for temperature*****************************/
        else if(txtMsg.charAt(0) == 't'){
            tempChar[0] = txtMsg.charAt(1);
            tempChar[1] = txtMsg.charAt(2);
            temp = (tempChar[0]-48)*10 + (tempChar[1]-48);
        }
        /****************setting for  font*************************************/
        /****************REQUIRES RESET*****************************************/
        else if(txtMsg.charAt(0) == 'f'){
            EEPROM.write(FONT,txtMsg.charAt(1)-48);
        }
        /////////UPDATING TIME REQUIRES RESTART
        else if(txtMsg.charAt(0) == 'm'){
            EEPROM.write(MSG1TIME,(txtMsg.charAt(1)-48)*10 + (txtMsg.charAt(2)-48));
        }
        
        else if(txtMsg.charAt(0) == 'n'){
            EEPROM.write(MSG2TIME,(txtMsg.charAt(1)-48)*10 + (txtMsg.charAt(2)-48));
        }
        
        ///////////////EMPTYIING THE BUFFER//////////////////////////
        txtMsg = "";
        }//END OF ONE LINE
    }//END OF SERIAL AVAILABLE
     Timer1.attachInterrupt( ScanDMD );
}

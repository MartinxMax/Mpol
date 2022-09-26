#include<reg52.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define uint unsigned int
#define uchar unsigned char
#define LC P0
sbit Beep=P1^0;
sbit LED=P1^1;
sbit Boom=P1^2;
sbit RS_1602=P1^5;
sbit RW_1602=P1^6;
sbit E_1602=P1^7;
uint Times=0;
char Keys[2];
uchar Key_num_pai[]={0x01,0x02,0x04};
uchar key_num_lie[]={0xE0,0xD0,0xB0,0x70} ;
uchar SMG[3][4]={
			 {'1','4','7','*'},
			 {'2','5','8','0'},
			 {'3','6','9','#'} };
void delay_ms(int MS){
	int is;
	while(MS--);

	for(is=0;is<1500;is++);
}
void delay(uint x)
{
uint i,j;
for(i=x;i>0;i--)
for(j=110;j>0;j--);
}

void time_start()
{
		EA=1;
		ET0=1;
		TMOD=0X01;
		TH0=(65536-50000)/256;
		TL0=(65536-50000)%256;
		TR0=1;
	   Times=0;
	}
 void LEDS(){
 LED=1;
 delay_ms(1000);
LED=0;
delay_ms(1000);
 }
void BeepS(){
Beep=1;
	delay_ms(1000);
Beep=0;
delay_ms(1000);
	}

 void Write_Cmd(uchar cmd)
{
RS_1602=0;
RW_1602=0;
LC=cmd;
delay(5);
E_1602=1;
delay(5);
E_1602=0;
delay(5);
}
 void Write_Dat(uchar Dat)
 {
 //Busy();
 RS_1602=1;
 RW_1602=0;	 
  //E_1602=0;
 LC=Dat;   
  	delay(5);
 E_1602=1;
   	delay(5);
 E_1602=0;
  delay(5);
 }
 void LCD1602_init()
 {
 RW_1602=0;
Write_Cmd(0x38);

Write_Cmd(0x0c);
Write_Cmd(0x06);
Write_Cmd(0x01);
 }
void Clear_Display(char chat,int C){
Write_Cmd(0x80+chat);
	while(C--){
	Write_Dat(0x00);
	delay(1);
	}
}
						
void Display(char* STR,char X,Y){
 Write_Cmd(0x80+(Y|X));
 while(*STR!='\0'){
	 Write_Dat(*STR++);
 }
}

int GetTime(int *x,int *y){
static int Record_digits=0;
char STR;
		 if(SMG[*x][*y]=='*'){Record_digits=0;Clear_Display(10,2);return 0;}
		 else if(SMG[*x][*y]=='#'){if(Record_digits==1){Keys[1]=Keys[0];Keys[0]='0';}Record_digits=0;return 1;}
		else if(Record_digits<2){
	       STR= Keys[Record_digits]=SMG[*x][*y];
		   Display(&STR,Record_digits+10,0);
		   Record_digits++;
		  }
		else{
		   return 0;
		}	  	 
}
int Key_input_time_module(){	
int Column_scan,Row_scan;
 Display("InputTime:",0,0);
  Display("(S)",12,0);
	while(1){
  			for(Column_scan=0;Column_scan<sizeof(Key_num_pai);Column_scan++)
			{
		    	for(Row_scan=0;Row_scan<sizeof(key_num_lie);Row_scan++)
				{
  		  			 P2 = Key_num_pai[Column_scan]|key_num_lie[Row_scan];				
					 if(P2==key_num_lie[Row_scan])
						{
						  		delay(20);
							if(P2==key_num_lie[Row_scan])
								{  
								while(P2==key_num_lie[Row_scan]);
								    //Display(&SMG[Column_scan][Row_scan],10,0);
									if(GetTime(&Column_scan,&Row_scan)==1){return 1;}

									}
								}				   



  				 }					  
			 }
  			}
}

void Explosion_Countdown(){
		     
		   int Total_duration =((Keys[0]-48)*10)+((Keys[1]-48)*1); 	  
		    
		   Display("Count_down:",0,0);
		   Display("(S)",13,0);
		   	 time_start();
			 while(Total_duration--){
			 BeepS();
			 	 Write_Cmd(0x80+0x0B);
				 Write_Dat(Total_duration/10+'0');
				 Write_Dat(Total_duration%10+'0');
				 while(Times<=20){LEDS();};
			 
				 Times=0;
						 }
				Beep=1;
				LED=1;
				  
				
}
 void Initialize_All(){
 EA=0;
 Boom=1;
 Beep=0;
 LED=0;

 LCD1602_init();
}
 
void main(){ 
Initialize_All();
Display("Martin-1.0",6,64);
Key_input_time_module();
Explosion_Countdown();
Boom=0;
while(1)if(Times==60){Beep=0;LED=0;break;};
}

 void ServiceTimer0()  interrupt 1
{  
  	 TH0=(65536-50000)/256;
     TL0=(65536-50000)%256;
	 Times++;
	  
  }

#include "reg52.h"
#define SEGPORT P0 //字形端口
#define BITPORT P2 //位选端口
sbit DQ=P3^3;
sbit beep = P3^7;
int readtemp=0;
unsigned char ledTab[]={0x3f,0x06,0x5b,0x4f,0x66,
						 0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char tabIndex[] = {0xFE,0xFD};
unsigned char DisplayData[2];

void Delay_Us(unsigned int us)//延时微秒级别
{
    while(us--);
}
void DynamicLed()
{
    unsigned char i;
    unsigned int n = 200;
    while(n--)
    {
		DisplayData[0] = ledTab[readtemp / 10];
        DisplayData[1] = ledTab[readtemp % 10];
        for(i = 0; i < 2; i ++)
        {
            Delay_Us(100);//实现灯亮度调节
            SEGPORT = 0x00;//实现单个LED的点亮
            BITPORT = tabIndex[i];            
            SEGPORT = DisplayData[i];
        }
    }
}
void delay_18B20(unsigned int i)
{
	for(;i>0;i--);
}

void Init_DS18B20(void) 	
{
  unsigned char x=0;
  DQ = 1;         
  delay_18B20(8);  
  DQ = 0;         
  delay_18B20(80); 
  DQ = 1;          
  delay_18B20(14);
  x=DQ;            
  delay_18B20(20);
}                 

unsigned char ReadOneChar(void)
  {
    unsigned char i=0;
    unsigned char dat = 0;
    for (i=8;i>0;i--) 
    {
      DQ = 0; 
      dat>>=1;
      DQ = 1; 
      if(DQ)
      dat|=0x80;
      delay_18B20(4);
     }
     return(dat);
  }
	
void WriteOneChar(unsigned char dat)
	
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
   DQ = 0;           
   DQ = dat&0x01;
   if(DQ){delay_18B20(1);DQ=1;}
	 else{delay_18B20(5);DQ = 1;}
   dat>>=1;
   }
}

unsigned char ReadTemperature(void)
{							 
 unsigned char a=0,b=0;
 unsigned int temp=0;
 Init_DS18B20();
 WriteOneChar(0xCC);  
 WriteOneChar(0x44);  
 delay_18B20(100);   
 Init_DS18B20();
 WriteOneChar(0xCC);  
 WriteOneChar(0xBE); 
 delay_18B20(100);
 a=ReadOneChar();     
 b=ReadOneChar();     
 temp=((b*256+a)>>4); 
 return(temp);
}

void main()
{
	beep = 0;
	while(1)
	{
		readtemp=ReadTemperature();
		DynamicLed();
		if(readtemp>30)
			beep = 1;
		else
			beep = 0;
	} 
}


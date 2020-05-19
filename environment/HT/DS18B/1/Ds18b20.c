#include "stm32f0xx.h"
#include "delay.h"

#define DQR GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
#define DQH GPIOA->BSRR = 0x0020
#define DQL GPIOA->BRR = 0x0020
#define uchar unsigned char

uint32_t T;


//初始化
void DS18B20init(){
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* 使能GPIOB时钟 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* 配置相应引脚PB1*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//重置 返回参数做测试
uint16_t Reset()                            
{
	uint16_t flag;
	DQH;
	delay_us(1);	//延时1us
	DQL;
	delay_us(500);	
	DQH;
	delay_us(45);
	flag=DQR;
	delay_us(70);
	DQH;
	delay_us(70);
	return flag;
}

//写数据
void Writedata(uint32_t dat)            
{ 
	uint16_t i;
	uint32_t flag;
	for(i=0;i<8;i++)
	{	
		//DQH;
		//delay_us(1);
		DQL;
		delay_us(2);
		DQH;
		delay_us(2);
		flag=dat&0x01;
		if(flag){
			DQH;
		}else{
			DQL;
		}
		delay_us(20);
		DQH;
		dat=dat>>1;
		//delay_us(2);
	}
}

//读数据
uchar Readdata()                    
{
	uchar i=0,dat=0;
	//uchar flag=0;
	for(i=0;i<8;i++)
	{
		//DQH;
		//delay_us(1);
		DQL;
		delay_us(4);
		DQH;
		//flag=DQR;
		//delay_us(30); 
		//DQH;
		delay_us(5);
		dat=dat>>1;
		if(DQR)
		{
			dat|=0x80;
		}
		delay_us(30);
		DQH;
		delay_us(1);
	}
	delay_us(20);
	return dat;
}

//返回温度
uint32_t ReadTem()                   
{
	uint16_t High,Low;
	Reset() ;
	delay_us(1000);
	Writedata(0xcc);
	//return Readdata();
	Writedata(0x44);
	delay_us(85);

	Reset();
	Writedata(0xcc);
	Writedata(0xbe);
	delay_us(85);

	Low=Readdata();
	High=Readdata();

	T=(((((High<<8)|Low)*0.0625)) * 10 )+ 0.5;

	return T;
}


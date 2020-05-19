#include"DHT11.h"
/*******************************************************************************
* Function Name  : 
* Description    : 
*******************************************************************************/

char Humi[6]={0};
char Temp[6]={0};
u8 cnt;

#define   DHT11_L   	     PA_ODR &= 0XF7  					
#define   DHT11_H	     PA_ODR |= 0X08 	
#define   DHT11_I 	     (0X08 & PA_IDR)	

void DHT11_Init(void)
{
	PA_CR1|=0X08;
	PA_CR2|=0X08;
	PA_ODR&=0XF7;
        PA_DDR|=0X08;
}
void DHT11_OUT(void)
{	
	//PA_CR2|=0X08;
	PA_ODR|=0X08;
	PA_DDR|=0X08;	
}

void DHT11_IN(void)
{	
        PA_CR2 &=0XF7;	
	//PA_CR1 |=0X08;
	PA_DDR &=0XF7;
}

int DHT11_Read_Byte(void)
{
	int Data = 0;
	for (cnt = 0; cnt < 8; cnt++)
	{
		while(DHT11_I==0);	
		Delay_us(30);
		if (DHT11_I) Data++;
		while(DHT11_I);		
		Data <<= 1;
	}
	return Data;
}

void DHT11(void) 
{
	int Data_Buff[5];
	DHT11_Init();
	Delay_ms(20);
	DHT11_H;
	DHT11_IN();
	Delay_us(30);
	if (DHT11_I==0)
	{
		while(DHT11_I==0);
		while(DHT11_I==1);
		for (cnt = 0; cnt< 5; cnt++) Data_Buff[cnt] = DHT11_Read_Byte();
		while(DHT11_I==0);
		DHT11_OUT();
		if ((Data_Buff[0] + Data_Buff[1] + Data_Buff[2] + Data_Buff[3])==Data_Buff[4])
		{			
		  	for(cnt=0;cnt<180;cnt++) 
			{
			  PB_ODR^=0X20;
			  Delay_ms(80);
			}
//			EEPROM_B(STORE_ADDR,Data_Buff[0]);
//			EEPROM_B(STORE_ADDR+1,Data_Buff[1]);
//			EEPROM_B(STORE_ADDR+2,Data_Buff[2]);
//			EEPROM_B(STORE_ADDR+3,Data_Buff[3]);
//			EEPROM_B(STORE_ADDR+4,0x77);
			Humi[0] = Data_Buff[0] / 10 + 48;
			Humi[1] = Data_Buff[0] % 10 + 48;
			Humi[2] = '.';
			Humi[3] = Data_Buff[1] / 10 + 48;
			Humi[4] = Data_Buff[1] % 10 + 48;

			Temp[0] = Data_Buff[2] / 10 + 48;
			Temp[1] = Data_Buff[2] % 10 + 48;
			Temp[2] = '.';
			Temp[3] = Data_Buff[3] / 10 + 48;
			Temp[4] = Data_Buff[3] % 10 + 48;
		}
	}

}


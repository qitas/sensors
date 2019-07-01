
#include "SI7021.h"
#include "i2c.h"
#include <stdlib.h>

float result_temp=0;
float result_humi=0;

u8 BUF[2]={0};
u16 result=0;
u8 SYS_Status=0;


void Init_SI7021(void)
{
	i2c_Port_Init();
}


void Cmd_Write_SI7021(u8 cmd)
{
    I2C_Start();                
    I2C_Send_Byte(SI7021_Addr+0); 
    if(I2C_Wait_Ack())  SYS_Status|=0x01;
    I2C_Send_Byte(cmd);          
    if(I2C_Wait_Ack()) SYS_Status|=0x02; 
    I2C_Stop();   
    Delay_ms(5);
}

void Read_SI7021(u8 time)
{   	
    I2C_Start();                        
    I2C_Send_Byte(SI7021_Addr+1);
    if(I2C_Wait_Ack())  SYS_Status|=0x02;
    Delay_ms(time);
    BUF[0]=I2C_Read_Byte(1); 
    BUF[1]=I2C_Read_Byte(0); 
    I2C_Stop();
    if(BUF[0]==0 && BUF[1]==0) 	SYS_Status|=0x02;
    Delay_ms(3);  
}

void Convert_Humi(void)
{
	Cmd_Write_SI7021(HUMI_HOLD_MASTER);
	Read_SI7021(25);
	result=BUF[0];
	result=(result<<8)+BUF[1];  
	result_humi=(result*125.0)/65536 - 6;
	if(result_humi<1||result_humi>90) 
	{
		 SYS_Status|=0x08;
		 EEPROM_B(MODE_ADDR+6,FLAG_ERROR);
	}
	else
	{	  	
		EEPROM_W(HUMI_ADDR+HUMI[0],BUF,2);		
		if(MODE[6]!=FLAG_OK)    EEPROM_B(MODE_ADDR+6,FLAG_OK); // flag humi
		//SYS_Status &=0xF0;
	}
}

void Convert_Temp(void)
{
	Cmd_Write_SI7021(TEMP_HOLD_MASTER);
	Read_SI7021(20);
	result=BUF[0];
	result=(result<<8)+BUF[1];  
	result_temp=(result*175.72)/65536 - 46.85;
	if(result_temp<-10||result_temp>85)
	{
	       SYS_Status|=0x04;	       
	       EEPROM_B(MODE_ADDR+5,(FLAG_ERROR));
	} 
	else
	{
		EEPROM_W(TEMP_ADDR+TEMP[0],BUF,2);		
		if(MODE[5]!=FLAG_OK)   EEPROM_B(MODE_ADDR+5,FLAG_OK); 
		//SYS_Status &=0xF0;
	}
}

u8 CHEC_TEMP(u8 dis)
{
        u8 buff,cnt;
	u16 check[5];
	for(cnt=0;cnt<5;cnt++)
	{
		Cmd_Write_SI7021(TEMP_HOLD_MASTER);
		Read_SI7021(20);
		result=BUF[0];
		check[cnt]=(result<<8)+BUF[1];  
		Delay_ms(300); 
	}
	result=0;
	for(cnt=0;cnt<5;cnt++)
	{
	  	for(buff=0;buff<5;buff++)
		{
		      result += abs(check[cnt]-check[buff]);
		}
	}
       buff=result>>8;
       EEPROM_B(M_ADDR+12,buff);
       buff=(u8)(result & 0XFF00);
       EEPROM_B(M_ADDR+13,buff);
	if(result>3745)  buff=0xFF;
	else buff=(u8)(result/16);
	return buff;
	
}

u8 CHEC_HUMI(u8 dis)
{
        u8 buff,cnt;
	u16 check[5];
	for(cnt=0;cnt<5;cnt++)
	{
		Cmd_Write_SI7021(HUMI_HOLD_MASTER);
		Read_SI7021(25);
		result=BUF[0];
		check[cnt]=(result<<8)+BUF[1];  
		Delay_ms(300); 
	}
	result=0;
	for(cnt=0;cnt<5;cnt++)
	{
	  	for(buff=0;buff<5;buff++)
		{
		      result += abs(check[cnt]-check[buff]);
		}
	}
	 buff=result>>8;
	 EEPROM_B(M_ADDR+14,buff);
	 buff=(u8)(result & 0XFF00);
	 EEPROM_B(M_ADDR+15,buff);
	 if(result>10480)  buff=0xFF;
	 else buff=(u8)(result/41);
	return buff;	
}

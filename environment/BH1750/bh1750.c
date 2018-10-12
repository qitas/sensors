

#include "BH1750.h"
#include "i2c.h"

u8 LX_BUF[2]={0};


void Init_BH1750(void)
{
	i2c_Port_Init();
	Delay_ms(15);
}

void Cmd_Write_BH1750(u8 cmd)
{
	I2C_Start();                 
	I2C_Send_Byte(BH1750_Addr+0);  
	while(I2C_Wait_Ack());
	I2C_Send_Byte(cmd);    
	 while(I2C_Wait_Ack());
	//BH1750_SendByte(REG_data);     
	I2C_Stop();              
	Delay_ms(5);
}

void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_RSET);	//clear
	Cmd_Write_BH1750(BH1750_ONE);  //一次H分辨率模式，至少120ms，之后自动断电模式  
}
void Read_BH1750(void)
{ 
	i2c_Port_Init();
	Start_BH1750();
	Delay_ms(150);
	I2C_Start();                   
	I2C_Send_Byte(BH1750_Addr+1);     
	while(I2C_Wait_Ack());
	LX_BUF[0]=I2C_Read_Byte(1);  
	LX_BUF[1]=I2C_Read_Byte(0);    
	I2C_Stop();             
	Delay_ms(5);
}
u16 Convert_BH1750(void)
{
        u16 result=0;
        Read_BH1750();
	//EEPROM_B(LX_ADDR,BUF[0]);
	//EEPROM_B(LX_ADDR+1,BUF[0]);
	UASRT_SEND(LX_BUF[0]);
	UASRT_SEND(LX_BUF[1]);
	result=LX_BUF[0];
	result=(result<<8)+LX_BUF[1];
	result=(u16)(result/1.2);
	return result;
}



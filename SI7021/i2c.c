
#include "i2c.h"


#define I2C_SCL_L   		GPIO_ResetBits(IIC_PORT , SCL_Pin)				
#define I2C_SCL_H		GPIO_SetBits(IIC_PORT , SCL_Pin)
#define I2C_SDA_L   		GPIO_ResetBits(IIC_PORT , SDA_Pin)				
#define I2C_SDA_H		GPIO_SetBits(IIC_PORT , SDA_Pin)
#define  SDA_I 			GPIO_ReadInputDataBit(IIC_PORT, SDA_Pin)

void i2c_Port_Init(void)
{
	GPIO_Init(IIC_PORT, SCL_Pin, GPIO_Mode_Out_OD_HiZ_Fast);
	GPIO_Init(IIC_PORT, SDA_Pin, GPIO_Mode_Out_OD_HiZ_Fast);
	I2C_SDA_H;
	I2C_SCL_H;
}

void SDA_OUT(void)
{
	GPIO_Init(IIC_PORT, SDA_Pin, GPIO_Mode_Out_OD_HiZ_Fast);
}

void SDA_IN(void)
{
	GPIO_Init(IIC_PORT, SDA_Pin, GPIO_Mode_In_PU_No_IT);
}

void I2C_Start(void)
{
	SDA_OUT();    
	I2C_SDA_H;
	I2C_SCL_H;
	Delay_us(4);
 	I2C_SDA_L;
	Delay_us(4);
	I2C_SCL_L;
}	  

void I2C_Stop(void)
{
	SDA_OUT();
	I2C_SCL_L;
	I2C_SDA_L;
 	Delay_us(4);
	I2C_SCL_H;
	I2C_SDA_H;
	Delay_us(4);	

}

u8 I2C_Wait_Ack(void)
{
	u8 ucErrTime=0;
//	SDA_IN();      //SDAÉèÖÃÎªÊäÈë  
	I2C_SDA_H;Delay_us(1);	   
	I2C_SCL_H;Delay_us(1);
	//
	SDA_IN();      //SDAÉèÖÃÎªÊäÈë	 
	while(SDA_I)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_SCL_L; 	   
	return 0;  
} 

void I2C_Ack(void)
{
	I2C_SCL_L;
	SDA_OUT();
	I2C_SDA_L;
	Delay_us(2);
	I2C_SCL_H;
	Delay_us(2);
	I2C_SCL_L;
}
	    
void I2C_NAck(void)
{
	I2C_SCL_L;
	SDA_OUT();
	I2C_SDA_H;
	Delay_us(2);
	I2C_SCL_H;
	Delay_us(2);
	I2C_SCL_L;
}					 				     
			  
void I2C_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT(); 	    
    I2C_SCL_L;
    for(t=8;t>0;t--)
    {          
        if(txd>=0x80) I2C_SDA_H;
	else  I2C_SDA_L;
        txd<<=1; 		  
		Delay_us(2);
		I2C_SCL_H;
		Delay_us(2); 
		I2C_SCL_L;	
		Delay_us(2);
    }	 
} 	    
  
u8 I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
    	for(i=0;i<8;i++ )
	{
	      I2C_SCL_L; 
	      Delay_us(2);
	      I2C_SCL_H;
	      receive<<=1;
	      if(SDA_I) receive++; Delay_us(1); 
    	}					 
    if (!ack) I2C_NAck();
    else   I2C_Ack(); 
    return receive;
}




#include "i2c.h"


#define I2C_SCL_L   	     PA_ODR &= 0XFD  				
#define I2C_SCL_H	     PA_ODR |= 0X02 	
#define I2C_SDA_L   	     PA_ODR &= 0XFB  					
#define I2C_SDA_H	     PA_ODR |= 0X04 	
#define  SDA_I 		     (0X04 & PA_IDR)	 


void i2c_Port_Init(void)
{
	PA_CR1|=0X04;
	PA_CR2|=0X06;
	PA_ODR|=0X06;
        PA_DDR|=0X06;
//	I2C_SDA_H;
//	I2C_SCL_H;
}
void SDA_OUT(void)
{	
	PA_CR2|=0X04;
	//PC_CR1|=0X80;
	PA_DDR|=0X04;	
}

void SDA_IN(void)
{	
        PA_CR2 &=0XFB;	
	PA_CR1 |=0X04;
	PA_DDR &=0XFB;
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
	u16 ucErrTime=0;
	I2C_SDA_H;
	Delay_us(2);	
	SDA_IN();    
	I2C_SCL_H;
	Delay_us(1);	   
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



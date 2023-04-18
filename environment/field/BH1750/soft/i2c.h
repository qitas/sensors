#ifndef __i2c_H
#define __i2c_H


void i2c_Port_Init(void);    		 
void I2C_Start(void);			
void I2C_Stop(void);	  			
void I2C_Send_Byte(u8 txd);			
u8 I2C_Read_Byte(unsigned char ack);
u8 I2C_Wait_Ack(void); 			
void I2C_Ack(void);					
void I2C_NAck(void);				
	  
#endif


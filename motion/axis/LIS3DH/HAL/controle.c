#include "defines.h"
#include "lis3dh_driver.h"
#include <math.h>

#define READ_LISD_REG 1
#define Transf_graus 57.29578//180 graus dividido por pi

float Angulo;
extern AxesRaw_t leitura;
extern AxesRaw_t leitura_inicial;
extern uint8_t StatusRegVar;
extern uint8_t StatusAUXVar;
extern uint16_t  DataReady ;   				  /*vari�vel respons�vel pela indica��o de ODReady*/


void LIS3DH_INT1_ISR(void)
{	
	LIS3DH_ResetInt1Latch();
	LIS3DH_GetStatusReg(&StatusRegVar);
	LIS3DH_GetStatusAUX(&StatusAUXVar);
	LIS3DH_GetAccAxesRaw(&leitura);
	DataReady = 1;
}

void LIS3DH_INT2_ISR(void)
{
	LIS3DH_ResetInt2Latch();
	LIS3DH_GetStatusReg(&StatusRegVar);
	LIS3DH_GetStatusAUX(&StatusAUXVar);
  LIS3DH_GetAccAxesRaw(&leitura);
}

void LIS3DH_Init(void)
{
	//Inicializa o acelerometro
	MEMS_Startup();
}


/**
*  Inicializa o acelerometro
**/
void MEMS_Startup (void)
{
	LIS3DH_GetWHO_AM_I(&WhoAmI);
	LIS3DH_WriteReg(LIS3DH_TEMP_CFG_REG, 0xC0);	
	LIS3DH_WriteReg(LIS3DH_CTRL_REG1, 0x57);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG2, 0x00);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG3, 0x50);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x80);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG5, 0x00);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG6, 0x00);	
	LIS3DH_WriteReg(LIS3DH_REFERENCE_REG, 0x00);	
	LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, 0x00);	
	LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x00);
	LIS3DH_WriteReg(LIS3DH_INT1_THS, 0x00);
	LIS3DH_WriteReg(LIS3DH_INT1_DURATION, 0x00);
	LIS3DH_WriteReg(LIS3DH_INT2_CFG, 0x00);	
	LIS3DH_WriteReg(LIS3DH_INT2_THS, 0x00);	
	LIS3DH_WriteReg(LIS3DH_INT2_DURATION, 0x00);	
	LIS3DH_WriteReg(LIS3DH_CLICK_CFG, 0x00);	
	LIS3DH_WriteReg(LIS3DH_CLICK_THS, 0x00);	
	LIS3DH_WriteReg(LIS3DH_TIME_LIMIT, 0x00);	
	LIS3DH_WriteReg(LIS3DH_TIME_LATENCY, 0x00);	
	LIS3DH_WriteReg(LIS3DH_TIME_WINDOW, 0x00);	
	LIS3DH_WriteReg(0X3E, 0x00);	
	LIS3DH_WriteReg(0X3F, 0x00);	
	
#ifdef READ_LISD_REG	
  LIS3DH_ReadReg(LIS3DH_CTRL_REG1 ,&Ctrl_reg1);
	LIS3DH_ReadReg(LIS3DH_CTRL_REG2 ,&Ctrl_reg2);
	LIS3DH_ReadReg(LIS3DH_CTRL_REG3 ,&Ctrl_reg3);
	LIS3DH_ReadReg(LIS3DH_CTRL_REG4 ,&Ctrl_reg4);
	LIS3DH_ReadReg(LIS3DH_CTRL_REG5 ,&Ctrl_reg5);
	LIS3DH_ReadReg(LIS3DH_CTRL_REG6 ,&Ctrl_reg6);
	LIS3DH_ReadReg(LIS3DH_INT1_CFG ,&Int1_cfg);
	LIS3DH_ReadReg(LIS3DH_INT1_THS ,&Int1_ths);
	LIS3DH_ReadReg(LIS3DH_INT1_DURATION ,&Int1_dur);	
#endif	
}

uint16_t Modulo (int_least16_t var){
		if(var < 0){
		var *=-1; //realiza o m�dulo
	}
	return var;
}



//angulo entre vetor instantaneo e vetor de referencia
float GetAngle (int_least16_t x1, int_least16_t y1 , int_least16_t z1, int_least16_t x2, int_least16_t y2 , int_least16_t z2)
{
	//int_least32_t n;
	float teta;
	float aux_teta,aux_teta1;
	float square_sum1; 
	float square_sum2;
	double aux_x1,aux_y1,aux_z1,aux_x2,aux_y2,aux_z2;	 
	aux_x1 = Modulo(x1); //calcula o modulo de x1
	aux_x1 *= aux_x1;    //calcula o quadrado de x1
	aux_y1 = Modulo(y1); //calcula o modulo de y1
	aux_y1 *= aux_y1;    //calcula o quadrado de y1
	aux_z1 = Modulo(z1); //calcula o modulo de z1
	aux_z1 *= aux_z1;    //calcula o quadrado de z1
	aux_x2 = Modulo(x2); //calcula o modulo de x2
	aux_x2 *= aux_x2;    //calcula o quadrado de x2
	aux_y2 = Modulo(y2); //calcula o modulo de y2
	aux_y2 *= aux_y2;    //calcula o quadrado de y2
	aux_z2 = Modulo(z2); //calcula o modulo de z2	
	aux_z2 *= aux_z2;    //calcula o quadrado de z2
	square_sum1 = sqrt(aux_x1 + aux_y1 + aux_z1); //calcula a raiz da soma dos quadrados do vetor de entrada
	square_sum2 = sqrt(aux_x2 + aux_y2 + aux_z2); //calcula a raiz da soma dos quadrados do vetor de referencia
    /****** produto escalar � dado por: A*B = |A|*|B|*cosO   ****/
	//inicio do claculo de A*B 
	aux_x1 = (int_least32_t)((int_least32_t)x1* (int_least32_t)x2);
	aux_y1 = (int_least32_t)((int_least32_t)y1* (int_least32_t)y2);
	aux_z1 = (int_least32_t)((int_least32_t)z1* (int_least32_t)z2);
	aux_teta = (aux_x1 + aux_y1 + aux_z1);//t�rmino do calculo de A*B
	aux_teta1 = (square_sum1*square_sum2);//calculo do produto |A|*|B| 
	//inicio do calculo de angulo teta
	teta = (float)(((float)aux_teta)/((float)aux_teta1)); 
	teta = acos(teta); //teta esta em radianos
	// arredondamento angular   
	teta = (float)(teta*(float)Transf_graus); //converte de radianos para graus (Transf_graus = 180/pi)
	//n = (teta - floor(teta) > 0.5) ? ceil(teta) : floor(teta); //usado no lugar da fun��o round() se fosse devolver um inteiro
	return (teta);
 }

#ifndef HX711_H_
#define HX711_H_

#include "gpio.h"

void        HX711_init(void);
int32_t     HX711_value(void);
int32_t     HX711_valueAve(uint16_t sample);

#endif 

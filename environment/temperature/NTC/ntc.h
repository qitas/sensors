#ifndef _NTC_H
#define _NTC_H

#include <stdint.h>

float ntc_convertToC(uint32_t adcValue);
float ntc_convertToF(uint32_t adcValue);
  
#endif


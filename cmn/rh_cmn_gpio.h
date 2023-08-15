#ifndef RH_BSP_LED_H
#define RH_BSP_LED_H

#include "rh_common.h"
#include "stm32f4xx.h"




u32 rh_cmn_gpio__readAll( void* GPIOx);
u32 rh_cmn_gpio__readBit( void* GPIOx, u32 pos);
u32 rh_cmn_gpio__setBit( void* GPIOx, u32 pos);

#endif
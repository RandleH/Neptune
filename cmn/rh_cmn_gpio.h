#ifndef RH_CMN_GPIO_H
#define RH_CMN_GPIO_H

#include "rh_common.h"

u32 rh_cmn_gpio__init( void);
u32 rh_cmn_gpio__readAll( void* GPIOx);
u32 rh_cmn_gpio__readBit( void* GPIOx, u32 pos);
u32 rh_cmn_gpio__setBit( void* GPIOx, u32 pos);
u32 rh_cmn_gpio__unsetBit( void* GPIOx, u32 pos);

#endif


#include "stm32f4xx.h"

#include "rh_cmn_gpio.h"




u32 rh_cmn_gpio__readAll( void* GPIOx){
    return ((GPIO_TypeDef *)GPIOx)->IDR;
}



u32 rh_cmn_gpio__readBit( void* GPIOx, u32 pos){
    return ((((GPIO_TypeDef *)GPIOx)->IDR) & pos);
}

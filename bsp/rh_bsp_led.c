/**
  ******************************************************************************
  * @file    rh_bsp_led.c
  * @author  RandleH
  * @brief   Provide APIs for board led control.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 RandleH.
  * All rights reserved.
  *
  * This software component is licensed by RandleH under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f411xe.h"        /* Description - STM32 Library */

#include "rh_bsp_led.h"         /* Path - ${PRJ_DIR}/bsp/rh_bsp.h */
#include "rh_cmn_mem.h"         /* Path - ${PRJ_DIR}/cmn/rh_cmn_mem.h */
#include "rh_cmn_gpio.h"        /* Path - ${PRJ_DIR}/cmn/rh_cmn_gpio.h */





/* Private define ------------------------------------------------------------*/
#define LED_R_GPIOx             GPIOA
#define LED_G_GPIOx             GPIOB
#define LED_B_GPIOx             GPIOC
#define LED_R_PINx              (11U)
#define LED_G_PINx              (12U)
#define LED_B_PINx              (13U)


#define CHECK_PARAM( pBspLedStruct, M_BSP_LED__xxxx )\
    do{\
        if( (pBspLedStruct)==NULL || ((M_BSP_LED__xxxx&(~M_BSP_LED__ALL))!=0) )\
            return UINT32_MAX;\
    }while(0)

#define SET_PARAM( pBspLedStruct, M_BSP_LED__xxxx )\
    do{\
        if( M_BSP_LED__xxxx & M_BSP_LED__R){\
            rh_cmn_gpio__setBit( LED_R_GPIOx, (u32)(1<<LED_R_PINx));\
        }\
        if( M_BSP_LED__xxxx & M_BSP_LED__G){\
            rh_cmn_gpio__setBit( LED_G_GPIOx, (u32)(1<<LED_G_PINx));\
        }\
        if( M_BSP_LED__xxxx & M_BSP_LED__B){\
            rh_cmn_gpio__setBit( LED_B_GPIOx, (u32)(1<<LED_B_PINx));\
        }\
    }while(0)

#define UNSET_PARAM( pBspLedStruct, M_BSP_LED__xxxx )\
    do{\
        if( M_BSP_LED__xxxx & M_BSP_LED__R){\
            rh_cmn_gpio__unsetBit( LED_R_GPIOx, (u32)(1<<LED_R_PINx));\
        }\
        if( M_BSP_LED__xxxx & M_BSP_LED__G){\
            rh_cmn_gpio__unsetBit( LED_G_GPIOx, (u32)(1<<LED_G_PINx));\
        }\
        if( M_BSP_LED__xxxx & M_BSP_LED__B){\
            rh_cmn_gpio__unsetBit( LED_B_GPIOx, (u32)(1<<LED_B_PINx));\
        }\
    }while(0)


/* Private typedef -----------------------------------------------------------*/
struct BspLed{

  u32 raw;
};


/* Functions -----------------------------------------------------------------*/

/**
 * @brief       Create a led handle
 * @return      Return NULL if failed
 *              Return valid handle if success
*/
pBspLed rh_bsp_led__init( void){
    pBspLed ptr = (pBspLed)rh_cmn_mem__malloc( sizeof(struct BspLed));
    
    #warning "TODO"
    return ptr;
}


u32     rh_bsp_led__switch( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay, u8 cmd){
    CHECK_PARAM(pContext, M_BSP_LED__xxxx);
    
    if(cmd==true){
        SET_PARAM( pBspLedStruct, M_BSP_LED__xxxx );
    }else{
        UNSET_PARAM( pBspLedStruct, M_BSP_LED__xxxx );
    }
    return 0;
}


u32     rh_bsp_led__toggle( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay){
    CHECK_PARAM(pContext, M_BSP_LED__xxxx);

    #warning "TODO"
    return 0;
}


u32     rh_bsp_led__state ( pBspLed pContext, u32 M_BSP_LED__xxxx){
    CHECK_PARAM(pContext, M_BSP_LED__xxxx);

    #warning "TODO"
    return 0;
}



u32    rh_bsp_led__deinit( pBspLed pContext){
    CHECK_PARAM(pContext, M_BSP_LED__ALL);

    #warning "TODO"
    rh_cmn_mem__free(pContext);
    return 0;
}

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
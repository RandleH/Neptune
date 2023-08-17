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
#include "rh_bsp_led.h"
#include "rh_cmn_mem.h"



/* Private define ------------------------------------------------------------*/
#define CHECK_PARAM( pBspLedStruct, M_BSP_LED__xxxx )\
    do{\
        if( (pBspLedStruct)==NULL || ((M_BSP_LED__xxxx&(~M_BSP_LED__ALL))!=0) )\
            return -1;\
    }while(0)




/* Private typedef -----------------------------------------------------------*/
struct BspLed{

  u32 raw;
};


/* Functions -----------------------------------------------------------------*/
pBspLed rh_bsp_led__init( void){
    pBspLed ptr = (pBspLed)rh_cmn_mem__malloc( sizeof(struct BspLed));
    
    #warning "TODO"
    return ptr;
}

u32     rh_bsp_led__switch( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay, u8 cmd){
    CHECK_PARAM(pContext, M_BSP_LED__xxxx);

    #warning "TODO"
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



void    rh_bsp_led__deinit( pBspLed pContext){
    CHECK_PARAM(pContext, M_BSP_LED__ALL);

    #warning "TODO"
    rh_cmn_mem__free(pContext);
}
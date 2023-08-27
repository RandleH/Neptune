/**
  ******************************************************************************
  * @file    main.c
  * @author  RandleH
  * @brief   Main program entrance
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

#include "rh_app_utility.h"
#include "rh_bsp.h"

#include "rh_cmn.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

extern u32 SystemCoreClock;


CmnData gCommonData = {0};



void delay( u32 ms){
    u64 tmp = ms+gCommonData.clockTick;
    while( gCommonData.clockTick < tmp){
        asm( "nop");
    }
}


int main( int argc, char const *argv[]){
    rh_cmn_clk__set_cpu( _96MHz);
    rh_cmn_clk__systick_enable( _1KHz);

    rh_cmn_usart__init( 115200);
    
    rh_cmn_usart__printf( "------------------------------------------------\n");
    rh_cmn_usart__printf( "System Core Frequency: %ld Hz\n", HAL_RCC_GetSysClockFreq());
    rh_cmn_usart__printf( "AHB Clock Frequency: %ld Hz\n", HAL_RCC_GetHCLKFreq());
    rh_cmn_usart__printf( "APB1 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK1Freq());
    rh_cmn_usart__printf( "APB2 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK2Freq());
    
    
    
    while(1){
        rh_cmn_delay_ms__halt(1000);
        rh_cmn_usart__printf( "ping\n");
        rh_cmn_delay_ms__halt(1000);
        rh_cmn_usart__printf( "pong\n");
    }

  return 0;
}

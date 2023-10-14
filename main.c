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


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "rh_bsp.h"
#include "rh_cmn.h"
#include "rh_app.h"
#include "rh_watch.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_cortex.h"

#include "FreeRTOS.h"
#include "task.h"

extern u32 SystemCoreClock;





/* Exported functions --------------------------------------------------------*/


int main( int argc, char const *argv[]){

    HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );
    rh_cmn_clk__set_cpu( kCmnCpuFreq_96MHz);
    rh_cmn_clk__systick_enable( kCmnSystickFreq_1KHz);

    xTaskCreate( watch.entrance, "Watch Program Entrance", 2048, NULL, 1, NULL);
    
    vTaskStartScheduler();

    while(1);

    return 0;
}


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
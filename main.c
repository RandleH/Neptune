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

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

#include "FreeRTOS.h"
#include "task.h"

extern u32 SystemCoreClock;





/* Exported functions --------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize ){
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize ){
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}



void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName ){

}


void vApplicationMallocFailedHook( void ){
    rh_cmn__assert( false, "FreeRTOS malloc failed!");
}



/**
 * @brief   LED Blink Task
 * @param    param cast to `AppLed_t *` aka. `struct AppLed *`
*/
void task_blink( void* param){
    static u8 cnt = 10;
    bool flag = true;
    while(1){
        vTaskDelay(400);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        g_AppTrace.printf( "LED: Turn on\r\n");
        
        vTaskDelay(400);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        g_AppTrace.printf( "LED: Turn off\r\n");
    
        if( cnt==0){
            if(flag==true){
                rh_cmn_clk__mco_disable();
                flag = false;
            }
        }else{
            --cnt;
        }
    }
}






void task_init( void *param){
    while(1){

        /* Peripheral Initialization */
        rh_cmn_gpio__init();                
        vTaskDelay(10);
        rh_cmn_usart__init( kCmnConst__USART_BAUDRATE);
        vTaskDelay(10);
        rh_cmn_spi__init( kCmnSpiFreq_48MHz);
        vTaskDelay(10);
        rh_cmn_clk__mco_disable();
        vTaskDelay(10);


        /* BSP Initialization */
        rh_bsp_screen__init();
        vTaskDelay(10);

        /* Application Sanity Test */
        /**
         * @note    If failed, infinite loop
         * @note    Run when `RH_APP_CFG__ENABLE_CI` enabled
         * @note    All application will be reset after the test
        */
#if defined (RH_APP_CFG__ENABLE_CI) && (((RH_APP_CFG__ENABLE_CI))==(true))
        extern int rh_app_trace__ci_sanity( void);
        
        if(0!=rh_app_trace__ci_sanity()){
            while(1);
        }
#endif

        /* Application Start */
        g_AppTrace.launch();
        g_AppGui.launch();
        
        /* Print General Information */
        CmnChipUID_t uid;
        rh_cmn_chip__uid( &uid);
        g_AppTrace.printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);

        g_AppTrace.printf( "CPU Info: System Core Frequency: %ld Hz\r\n", HAL_RCC_GetSysClockFreq());
        g_AppTrace.printf( "CPU Info: AHB Clock Frequency: %ld Hz\r\n", HAL_RCC_GetHCLKFreq());
        g_AppTrace.printf( "CPU Info: APB1 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK1Freq());
        g_AppTrace.printf( "CPU Info: APB2 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK2Freq());

        /* Initialization Completed */
        vTaskDelete(NULL);
    }
}



#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"

#include "rh_app.h"



int main( int argc, char const *argv[]){

    HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );
    rh_cmn_clk__set_cpu( kCmnCpuFreq_96MHz);
    rh_cmn_clk__systick_enable( kCmnSystickFreq_1KHz);

    xTaskCreate( task_init, "Init task", 8192U, NULL, 40U, NULL);
    
    vTaskStartScheduler();

    while(1);

    return 0;
}


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
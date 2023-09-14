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
#include "rh_app.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

#include "FreeRTOS.h"
#include "task.h"

extern u32 SystemCoreClock;






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
    
}



void task( void* param){

    while(1){
        vTaskDelay(1000);
        
        taskENTER_CRITICAL();
        rh_cmn_usart__printf( "ping\n");
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        taskEXIT_CRITICAL();

        vTaskDelay(1000);
        
        taskENTER_CRITICAL();
        rh_cmn_usart__printf( "pong\n");
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        taskEXIT_CRITICAL();
    }
}


void task_print_cpu_info( void* param){
    while(1){
        taskENTER_CRITICAL();
        rh_cmn_usart__printf( "------------------------------------------------\n");
        rh_cmn_usart__printf( "System Core Frequency: %ld Hz\n", HAL_RCC_GetSysClockFreq());
        rh_cmn_usart__printf( "AHB Clock Frequency: %ld Hz\n", HAL_RCC_GetHCLKFreq());
        rh_cmn_usart__printf( "APB1 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK1Freq());
        rh_cmn_usart__printf( "APB2 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK2Freq());
        taskEXIT_CRITICAL();

        xTaskCreate( task, "USART task", 1024U, NULL, 40U, NULL);
        vTaskDelete(NULL);
    }
}



void task_init( void *param){
    while(1){
        rh_cmn_gpio__init();                
        vTaskDelay(10);
        rh_cmn_usart__init( 115200);
        vTaskDelay(10);
        rh_cmn_spi__init( kCmnSpiFreq_48MHz);
        vTaskDelay(10);
        rh_bsp_screen__init();
        vTaskDelay(10);

        xTaskCreate( task_print_cpu_info, "USART task", 1024U, NULL, 40U, NULL);
        vTaskDelete(NULL);
    }
}





int main( int argc, char const *argv[]){
    rh_cmn_clk__set_cpu( kCmnCpuFreq_96MHz);
    rh_cmn_clk__systick_enable( kCmnSystickFreq_1KHz);
    
    

    // rh_app_trace__init(NULL);
    // rh_app_trace__append_message( kTraceBusClock, "------------------------------------------------\n");
    // rh_app_trace__append_message( kTraceBusClock, "System Core Frequency: %ld Hz\n", HAL_RCC_GetSysClockFreq());
    // rh_app_trace__append_message( kTraceBusClock, "AHB Clock Frequency: %ld Hz\n", HAL_RCC_GetHCLKFreq());
    // rh_app_trace__append_message( kTraceBusClock, "APB1 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK1Freq());
    // rh_app_trace__append_message( kTraceBusClock, "APB2 Clock Frequency: %ld Hz\n", HAL_RCC_GetPCLK2Freq());
    

    
    xTaskCreate( task_init, "Init task", 4096U, NULL, 40U, NULL);
    
    
    vTaskStartScheduler();

    while(1);

    return 0;
}

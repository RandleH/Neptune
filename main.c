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
    rh_cmn__assert( false, "FreeRTOS malloc failed!");
}




void task_blink( void* param){
    static u8 cnt = 10;
    bool flag = true;
    while(1){
        vTaskDelay(1000);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        
        vTaskDelay(1000);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
    
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


#include <wchar.h>

u16 g_Gram[30*30] = {0};

void task_switch_color( void *param){
    const u16 color[] = { 0xF800, 0x07E0, 0x001F};
    u8 idx = 0;
    while(1){
        rh_cmn_memset16( g_Gram, color[idx%3], 30*30);
        rh_bsp_screen__flush( g_Gram, 200, 100, 230-1, 130-1);
        ++idx;
        vTaskDelay(1000);
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

        vTaskDelete(NULL);
    }
}


#include <string.h>


void task( void* param){
    g_AppTrace.launch();

    while(1){
        g_AppTrace.main( 0, 0);
        vTaskDelay(1000);
    }

    g_AppTrace.exit();
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

        

        rh_cmn_clk__mco_enable();

        xTaskCreate( task_print_cpu_info, "USART task", 1024U, NULL, 40U, NULL);
        xTaskCreate( task_switch_color,   "Screen task", 1024U, NULL, 45U, NULL);
        xTaskCreate( task_blink, "USART task", 1024U, NULL, 40U, NULL);
        xTaskCreate( task, "App Trace task", 1024U, NULL, 40U, NULL);
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
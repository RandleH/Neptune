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


/**
 * @brief   System Report
 * @param   param       Call back `printf()`. Must NOT be NULL.
*/
void task_report( void* param){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 5000;
     xLastWakeTime = xTaskGetTickCount();

    typedef int (*printf_t)(const char*,...);
    HeapStats_t report_heap;
    vPortGetHeapStats( &report_heap);
    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );


        ((printf_t)param)( "============================================================\n");
        ((printf_t)param)( "System Report\n");
        ((printf_t)param)( "============================================================\n");
        ((printf_t)param)( "Heap Memory Usage ------------------------------------------\n");
        ((printf_t)param)( " - Number of Free Memory Blocks:              %ld\tbytes\n", report_heap.xNumberOfFreeBlocks);
        ((printf_t)param)( " - Minimum Remaining Free Bytes Since Boot:   %ld\tbytes\n", report_heap.xMinimumEverFreeBytesRemaining);
        ((printf_t)param)( " - Maximum Allocable Bytes:                   %ld\tbytes\n", report_heap.xSizeOfLargestFreeBlockInBytes);
        ((printf_t)param)( " - Total Remaining Free Bytes:                %ld\tbytes\n", report_heap.xAvailableHeapSpaceInBytes);
        ((printf_t)param)( " - Total Heap Size:                           %ld\tbytes\n", configTOTAL_HEAP_SIZE);
        ((printf_t)param)( " - Number of calls to pvPortMalloc()          %ld\t\n", report_heap.xNumberOfSuccessfulAllocations);
        ((printf_t)param)( " - Number of calls to pvPortFree()            %ld\t\n", report_heap.xNumberOfSuccessfulFrees);
        ((printf_t)param)( "\n\n");

        ((printf_t)param)( "Task Statistic ---------------------------------------------\n");
        ((printf_t)param)( " - Total Number of Tasks                      %ld\t\n", uxTaskGetNumberOfTasks());
        
        ((printf_t)param)( " -- Task Name:                                %s\n", pcTaskGetName( NULL));
        ((printf_t)param)( " -- Task Stack Peak Usage:                    %ld/%ld\n", sizeof(StackType_t)*(128-uxTaskGetStackHighWaterMark2( NULL)), 128*sizeof(StackType_t));
        ((printf_t)param)( "\n\n""\033[0m");


        // ulTaskGetIdleRunTimePercent();

        // ulTaskGetRunTimePercent(  )
        
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
        // g_AppGui.launch();
        
        /* Print General Information */
        CmnChipUID_t uid;
        rh_cmn_chip__uid( &uid);
        g_AppTrace.printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);

        g_AppTrace.printf( "CPU Info: System Core Frequency: %ld Hz\r\n", HAL_RCC_GetSysClockFreq());
        g_AppTrace.printf( "CPU Info: AHB Clock Frequency: %ld Hz\r\n", HAL_RCC_GetHCLKFreq());
        g_AppTrace.printf( "CPU Info: APB1 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK1Freq());
        g_AppTrace.printf( "CPU Info: APB2 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK2Freq());
        
        xTaskCreate( task_blink, "Task - LED", 256U, NULL, 30, NULL);
        xTaskCreate( task_report, "Task - System Report", 256, g_AppTrace.printf, 30, NULL);
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

    xTaskCreate( task_init, "Init task", 512U, NULL, 40U, NULL);
    
    vTaskStartScheduler();

    while(1);

    return 0;
}


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
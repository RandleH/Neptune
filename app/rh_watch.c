/**
  ******************************************************************************
  * @file    rh_watch.c
  * @author  RandleH
  * @brief   Top source code for Watch OS
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
#include <string.h>

#include "rh_watch.h"
#include "rh_cmn.h"


#ifdef __cplusplus
extern "C"{
#endif

/* Private function prototypes -----------------------------------------------*/

static void bsp_init_function( void);
static void sys_init_function( void);
static void entrance_function( void* param);



/* Private functions ---------------------------------------------------------*/

/**
 * @brief   Top init function for BSP
 * @note    This function should only be called from `entrance()`
 * @note    All other task will be blocked during the function
 * @refitem     GPIO | SPI | USART | RTC | MCO | LCD SCREEN
*/
static void bsp_init_function( void){
    taskENTER_CRITICAL();
    /* Print General Information */
    CmnChipUID_t uid;
    rh_cmn_chip__uid( &uid);
    watch.sys.logger->printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);

    watch.sys.logger->printf( "CPU Info: System Core Frequency: %ld Hz\r\n", HAL_RCC_GetSysClockFreq());
    watch.sys.logger->printf( "CPU Info: AHB Clock Frequency: %ld Hz\r\n", HAL_RCC_GetHCLKFreq());
    watch.sys.logger->printf( "CPU Info: APB1 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK1Freq());
    watch.sys.logger->printf( "CPU Info: APB2 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK2Freq());
    watch.sys.logger->printf("\n\n"); 


    int ret;
    
    watch.sys.logger->printf("Hardware Initializing...\n");
    /* Peripheral Initialization */
    watch.sys.logger->printf("GPIO ........................................ ");
    ret = rh_cmn_gpio__init();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);
    
    watch.sys.logger->printf("USART ....................................... ");
    ret = rh_cmn_usart__init( kCmnConst__USART_BAUDRATE);
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);
    
    watch.sys.logger->printf("SPI ......................................... ");
    ret = rh_cmn_spi__init( kCmnSpiFreq_48MHz);
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);
    
    watch.sys.logger->printf("MCO ......................................... ");
    ret = rh_cmn_clk__mco_disable();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);

    watch.sys.logger->printf("RTC ......................................... ");
    ret = rh_cmn_rtc__init(RH_TIMESTAMP);
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);
    watch.sys.logger->printf("\n\n");


    /* BSP Initialization */
    watch.sys.logger->printf("Device Initializing...\n");
    watch.sys.logger->printf("Screen ...................................... ");
    ret = rh_bsp_screen__init();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);

    watch.sys.logger->printf("\n\n"); 
    taskEXIT_CRITICAL();
}

/**
 * @brief   Top init function for System
 * @note    This function should only be called from `entrance()`
 * @note    All other task will be blocked during the function
*/
static void sys_init_function( void){
    taskENTER_CRITICAL();
    
    int ret;
    watch.sys.logger->printf("System Initializing...\n");

    watch.sys.logger->printf("Task Manager ................................ ");
    ret = watch.sys.taskmgr->launch();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);

    watch.sys.logger->printf("Trace ....................................... ");
    ret = watch.sys.logger->launch();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);

    watch.sys.logger->printf("GUI ......................................... ");
    ret = watch.sys.gui->launch();
    (ret==OK)? watch.sys.logger->printf("success\n") : watch.sys.logger->printf("failed\t Returned value is %d\n", ret);
    
    watch.sys.logger->printf("\n\n");
    taskEXIT_CRITICAL();

}

/**
 * @brief   Top custom function 
 * @note    This function should only be called from `entrance()`
 * @note    For debug use only. Will be ignored in the release version
*/
static void custom_function( void* param){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 30000;

    xLastWakeTime = xTaskGetTickCount();

    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        watch.sys.logger->printf("RTC Report Time: %s\n", rh_cmn_rtc__report());

        
    }
}

/**
 * @brief   Watch Entrance Function
 * @note    All process comes from this function
 * @note    The FreeRTOS will execute the program from this. It is equvalent to a boot program.
 * @warning CI program was not integrated
*/
static void entrance_function( void* param){
    (void)param;

    /* Application Sanity Test */
    /**
     * @note    If failed, infinite loop
     * @note    Run when `RH_APP_CFG__ENABLE_CI` enabled
     * @note    All application will be reset after the test
    */
#if defined (RH_APP_CFG__ENABLE_CI) && (((RH_APP_CFG__ENABLE_CI))==(true))
    // extern int rh_app_trace__ci_sanity( void);
    
    // if(0!=rh_app_trace__ci_sanity()){
    //     while(1);
    // }
#endif

    /* BSP Initialization */
    bsp_init_function();

    /* System Initialization */
    sys_init_function();

    
    /* Print all message on screen */
    watch.sys.logger->cache_mode(false);

    AppTaskUnit_t list[] = {
        {
            .pcName = "Clock Model",
            .pvParameters = NULL,
            .pvTaskCode = watch.app.clock->isDisplayable->model.main_func,
            .pvTaskExitCode = watch.app.clock->isDisplayable->model.exit_func,
            .usStackDepth = 512,
            .uxPriority = kAppConst__PRIORITY_IMPORTANT,
        },
        {
            .pcName = "Clock View",
            .pvParameters = NULL,
            .pvTaskCode = watch.app.clock->isDisplayable->visual.main_func,
            .pvTaskExitCode = watch.app.clock->isDisplayable->visual.exit_func,
            .usStackDepth = 512,
            .uxPriority = kAppConst__PRIORITY_IMPORTANT,
        },
        {
            .pcName = "Clock Ctrl",
            .pvParameters = NULL,
            .pvTaskCode = watch.app.clock->isDisplayable->ctrl.main_func,
            .pvTaskExitCode = watch.app.clock->isDisplayable->ctrl.exit_func,
            .usStackDepth = 512,
            .uxPriority = kAppConst__PRIORITY_IMPORTANT,
            
        }
    };
    watch.sys.taskmgr->create( list, 3);

    watch.app.clock->isDisplayable->model.handle  = list[0].pxCreatedTask;
    watch.app.clock->isDisplayable->visual.handle = list[1].pxCreatedTask;
    watch.app.clock->isDisplayable->ctrl.handle   = list[2].pxCreatedTask;

     
    list[0].pcName = "RTC report";
    list[0].pvParameters = NULL;
    list[0].pvTaskCode = custom_function;
    list[0].usStackDepth = 512;
    list[0].uxPriority = kAppConst__PRIORITY_DOCUMENTATION;
        
    watch.sys.taskmgr->create( list, 1);


    vTaskDelete(NULL);
}

#ifdef __cplusplus
}
#endif    


/* Exported constants --------------------------------------------------------*/
const WatchTopStructure_t watch = {
    .entrance = entrance_function,
    .app = {
        .clock    = &g_AppClock
    },
    .sys = {
        .gui      = &g_AppGui,
        .logger   = &g_AppTrace,
        .taskmgr  = &g_AppTaskMgr,
    },
    .hw = {
        .rtc      = &g_CmnRtc,
        .spi      = &g_CmnSpi,
        .usart    = &g_CmnUsart,
        .screen   = &g_BspScreen
    }
};

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
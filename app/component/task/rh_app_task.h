/**
  ******************************************************************************
  * @file    rh_app_trace.h
  * @author  RandleH
  * @brief   Application: Trace
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

#ifndef RH_APP_TASK_H
#define RH_APP_TASK_H


/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rh_common.h"      /* Description - Project Common Headfile */
#include "rh_cmn.h"         /* Description - Common Components */
#include "rh_bsp.h"         /* Description - Board Support Package Components */
#include "rh_app.h"         /* Description - Application Components */


typedef struct AppTaskUnit{
    TaskFunction_t      pvTaskCode;
    TaskFunction_t      pvTaskExitCode;
    const char *        pcName;
    StackType_t         usStackDepth;
    void               *pvParameters;
    UBaseType_t         uxPriority;
    TaskHandle_t        pxCreatedTask;
}AppTaskUnit_t;


typedef struct AppTaskUnitInternal{
    TaskFunction_t      func;
    TaskFunction_t      exit_func;
    void*               param;
    void*               exit_param;
    TaskHandle_t        handle;
    size_t              depth;
}AppTaskUnitInternal_t;


/**
 * @brief       Application Super Structure
 * @note        Globally created in program
 *              size: ? bytes
 * @attention   Do NOT change any value. Use api function instead
 *              Do NOT define in any functions.
*/
typedef struct AppTaskMgr{

    /* Private -----------------------------------------------------------*/
    AppTaskUnitInternal_t               *tc_list;
    u8                                  *tc_list_mask;              /*!< 0=BUSY; 1=IDLE */
    u8                                   tc_list_mask_len;
    
    TaskHandle_t                         task_report;

    /* Public ------------------------------------------------------------*/
    /**
     * @brief   External Function: `TaskMgr` launch function.
     * @note    `launch()` should run before all other external function was called
     * @retval  Return 0 if success   
    */
    int (*launch)( void);

    /**
     * @brief   External Function: Create a list of tasks.
     * @note    This function may cause memory reallocation inside the `TaskMgr`
     * @retval  Return 0 if success.
    */
    int (*create)( AppTaskUnit_t list[], size_t nItems );
    
    /**
     * @brief   External Function: Generate human readable report of all tasks created by `TaskMgr`
     * @note    This function will call `AppTrace` application to help export the message
     * @note    `RH_APP_CFG__TASK_MGR_DEBUG` MUST set to `1`
     * @retval  Return 0 if success.
    */
    

    /**
     * @brief   External Function: Given a task handle, kill the task.
     * @param   t           Task handle
     * @param   status      Return status
     * @retval  Return 0 if success
    */
    int (*kill)( TaskHandle_t t, int status);

}AppTask_t;


extern AppTask_t g_AppTaskMgr;

#endif
/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
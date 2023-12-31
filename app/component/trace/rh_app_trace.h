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

#ifndef RH_APP_TRACE_H
#define RH_APP_TRACE_H


/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rh_common.h"      /* Description - Project Common Headfile */
#include "rh_cmn.h"         /* Description - Common Components */
#include "rh_bsp.h"         /* Description - Board Support Package Components */
#include "rh_app.h"         /* Description - Application Components */


#define PER_BUFFER_SIZE     (1<<(kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL-5))

/* Exported types ------------------------------------------------------------*/
typedef struct AppTraceUnit{
    struct AppTraceUnit *pNext;
    size_t               len;       /*!< Buffer valid length in string */
    u8                   addr[PER_BUFFER_SIZE];
}AppTraceUnit_t;



/**
 * @brief       Dummy linked list. Only for head and end node usage
 * @note        `pHead` & `pEnd` conditions are mutually exclusive
 *              size: ? bytes
 * @attention   Never use anchor to fetch content since `AppTraceUnit_t` pointer cast to anchor. 
 * @param       phead       Head Node - If `NULL`, then linked list is empty
 * @param       pEnd        End Node  - If pointing to anchor itself, then linked list is empty
*/
typedef struct AppTraceUnitAnchor{
    struct AppTraceUnit *pHead;
    struct AppTraceUnit *pEnd;
}AppTraceUnitAnchor_t;


enum AppTraceOutClient{
    kAppTraceOutClient__usart,          /*!< Output message throught usart port */
    kAppTraceOutClient__null            /*!< No output */
};


/**
 * @brief       Application Super Structure
 * @note        Globally created in program
 *              size: ? bytes
 * @attention   Do NOT change any value. Use api function instead
 *              Do NOT define in any functions.
*/
typedef struct AppTrace{
    /* Private -----------------------------------------------------------*/
    TaskHandle_t    task_tx;
    TaskHandle_t    task_rx;

    struct{
        u32                     mask_rx;   /*!< 0=Busy; 1=Idle */
        u32                     mask_tx;   /*!< 0=Busy; 1=Idle */
        AppTraceUnit_t          slot[32];  /*!< Entire Buffer */
        AppTraceUnitAnchor_t    anchor;    /*!< Link list dummy head & end node */
    }buffer;

    SemaphoreHandle_t lock_handle;
    StaticSemaphore_t lock_buffer;
    uint8_t           force_to_clear;
    uint8_t           isCacheMode;


    /* Public ------------------------------------------------------------*/
    int (*launch)( void);
    
    void (*cache_mode)( bool cmd);
    int (*printf)( const char *fmt, ...);
    int (*purge)(void);
    int (*flush)(void);
    int (*exit)(int);

}AppTrace_t;


/* Exported variable ---------------------------------------------------------*/
extern AppTrace_t g_AppTrace;

/**
 * @note    Must enable `RH_APP_CFG__ENABLE_CI` to call this function
 * @return  Return 0 if passed
*/
extern int rh_app_trace__ci_sanity( void);


#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
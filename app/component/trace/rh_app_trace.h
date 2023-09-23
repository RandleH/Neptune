/**
  ******************************************************************************
  * @file    rh_app_trace.h
  * @author  RandleH
  * @brief   Application: Message Trace
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
#include "rh_bsp.h"
#include "rh_app.h"



typedef struct AppTraceUnit{
    struct AppTraceUnit *pNext;
    struct AppTraceUnit *pPrev;
    size_t               len;       /*!< Buffer valid length in string */
    u8                   addr[1<<(kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL-5)];
}AppTraceUnit_t;

typedef struct AppTraceUnitAnchor{
    struct AppTraceUnit *pHead;
    struct AppTraceUnit *pEnd;
}AppTraceUnitAnchor_t;




typedef struct AppTrace{
    /* Private -----------------------------------------------------------*/
    TaskHandle_t    task_tx;
    TaskHandle_t    task_rx;

    StackType_t     stack[kAppConst__TRACE_STATIC_STACK_SIZE];

    struct{
        u32                     mask_rx;   /*!< 0=Busy; 1=Idle */
        u32                     mask_tx;   /*!< 0=Busy; 1=Idle */
        AppTraceUnit_t          data[32];  /*!< Entire Buffer */
        AppTraceUnitAnchor_t    anchor;    /*!< Link list dummy head & end node */
    }activity;

    SemaphoreHandle_t lock_handle;
    StaticSemaphore_t lock_buffer;
    uint8_t           force_to_clear;


    /* Public ------------------------------------------------------------*/
    u32 (*launch)( void);
    int (*main)( int argc, const char*argv[]);
    u32 (*message)( const char *fmt, ...);
    u32 (*isEmptyTX)( void);
    u32 (*isEmptyRX)( void);

    int (*exit)(void);

}AppTrace_t;

extern AppTrace_t g_AppTrace;




#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
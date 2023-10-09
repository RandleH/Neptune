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

}AppTaskUnit_t;

typedef struct AppTaskMemory{

}AppTaskMemory_t;


/**
 * @brief       Application Super Structure
 * @note        Globally created in program
 *              size: ? bytes
 * @attention   Do NOT change any value. Use api function instead
 *              Do NOT define in any functions.
*/
typedef struct AppTask{

    

    /* Public ------------------------------------------------------------*/
    u32 (*launch)( void);
    
    
    int (*schedule)( const AppTaskUnit_t[] list, size_t nItems );
    
    int (*report)(void);
    int (*memory)( AppTaskMemory_t *result);

    int (*reset)(void);
    int (*exit)(int);

}AppTask_t;



/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
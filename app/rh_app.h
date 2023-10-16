/**
  ******************************************************************************
  * @file    rh_app.h
  * @author  RandleH
  * @brief   Application headfiles.
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

#ifndef RH_APP_H
#define RH_APP_H


/* Exported macro ------------------------------------------------------------*/
#define RH_APP_CFG__ENABLE_CI                       (true)
#define RH_APP_CFG__DEBUG                           (true)
#define RH_APP_CFG__TRACE__ENABLE_TX                (true)
#define RH_APP_CFG__TRACE__ENABLE_RX                (true)

#if RH_APP_CFG__DEBUG

#define RH_APP_CFG__TASK_MGR_DEBUG                  (true)
#define RH_APP_CFG__TRACE_DEBUG                     (true)

# define RH_APP_CFG__TRACE__ENABLE_STACK_WATERMARK  (true)

#endif




/* Exported types ------------------------------------------------------------*/
enum AppConst{
    kAppConst__TRACE_STACK_DEPTH                   = (1024),
    kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL = (12),     // 2^12 = 4096
    kAppConst__TRACE_PRIORITY                      = (20),
    kAppConst__TRACE_MAX_WAIT_TICK                 = (50),
    kAppConst__GUI_STACK_DEPTH                     = (1024),
    kAppConst__GUI_FRAME_RATE_MS                   = (10),
    kAppConst__GUI_NUM_OF_GRAM                     = (2),
    kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM           = (240*4),
    kAppConst__GUI_PRIORITY                        = (50),


    kAppConst__PRIORITY_VERY_IMPORTANT             = (50),
    kAppConst__PRIORITY_IMPORTANT                  = (40),
    kAppConst__PRIORITY_INTERMEDIATE               = (30),
    kAppConst__PRIORITY_NORMAL                     = (20),
    kAppConst__PRIORITY_DOCUMENTATION              = (5)
};


/* Includes ------------------------------------------------------------------*/
#include "rh_app_trace.h"
#include "rh_app_utility.h"
#include "rh_app_task.h"
#include "rh_app_time.h"
#include "rh_app_gui.h"


#endif
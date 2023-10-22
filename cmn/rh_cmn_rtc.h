/**
  ******************************************************************************
  * @file    rh_cmn_rtc.h
  * @author  RandleH
  * @brief   Driver source code for RTC
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


#ifndef RH_CMN_RTC_H 
#define RH_CMN_RTC_H


/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "rh_common.h"

#ifdef __cplusplus
extern "C"{
#endif


/* Exported types ------------------------------------------------------------*/

typedef struct CmnRtc{
    RTC_HandleTypeDef        hw_handle;
    RTC_DateTypeDef          date;
    RTC_TimeTypeDef          time;
    
}CmnRtc_t;


/* Exported functions --------------------------------------------------------*/

int rh_cmn_rtc__init( const char* time_stamp);
const char* rh_cmn_rtc__report( void);
void rh_cmn_rtc__update( void);
int rh_cmn_rtc__set( u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second);


/* Exported variables --------------------------------------------------------*/

extern CmnRtc_t g_CmnRtc;


#ifdef __cplusplus
}
#endif /* End of `__cplusplus` */

#endif /* End of `RH_CMN_RTC_H` */

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
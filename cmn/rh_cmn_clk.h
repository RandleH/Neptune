/**
  ******************************************************************************
  * @file    rh_cmn_clk.h
  * @author  RandleH
  * @brief   Configuration for CPU clock speed.
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

#ifndef RH_CMN_CLK_H
#define RH_CMN_CLK_H 

/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"



#ifdef __cplusplus
extern "C"{
#endif




/* Public typedef ------------------------------------------------------------*/
enum CmnCpuFreq{
  kCmnCpuFreq_16MHz = 0,
  kCmnCpuFreq_25MHz = 1,
  kCmnCpuFreq_32MHz = 2,
  kCmnCpuFreq_96MHz = 3,
  kCmnCpuFreq_END
};

enum CmnUsbFreq{
  kCmnUsbFreq_48MHz = 0,         /*!< Default Bus Clock for USB */
  kCmnUsbFreq_12MHz = 1          /*!< Full Speed Bus Clock for USB */
};

enum CmnSystickFreq{
  kCmnSystickFreq_1KHz   = 0,
  kCmnSystickFreq_100Hz  = 1,
  kCmnSystickFreq_10Hz   = 2
};

typedef struct CmnClkEpoch *pCmnClkEpoch;


/* Exported functions --------------------------------------------------------*/
u32 rh_cmn_clk__set_cpu  ( enum CmnCpuFreq frequency );
u32 rh_cmn_clk__set_usb  ( enum CmnUsbFreq frequency );

enum CmnCpuFreq rh_cmn_clk__get_cpu  ( void);



u32 rh_cmn_clk__reset( void);
u32 rh_cmn_clk__mco_enable  ( void);
u32 rh_cmn_clk__mco_disable ( void);


u32          rh_cmn_clk__systick_enable( enum CmnSystickFreq frequency);
pCmnClkEpoch rh_cmn_clk__systick_create_epoch( void);
u32          rh_cmn_clk__systick_duration_epoch( pCmnClkEpoch pEpoch, bool update);
u32          rh_cmn_clk__systick_update_epoch( pCmnClkEpoch pEpoch);
void         rh_cmn_clk__systick_delete_epoch( pCmnClkEpoch pEpoch);


u32 rh_cmn_clk__systick_now( void);

#ifdef __cplusplus
}
#endif

#endif
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
  _16MHz = 0,
  _25MHz = 1,
  _32MHz = 2,
  _48MHz = 3,
  _50MHz = 4,
  _96MHz = 5
};



/* Exported functions --------------------------------------------------------*/
u32 rh_cmn_clk__cpu  ( enum CmnCpuFreq frequency );
u32 rh_cmn_clk__reset( void);
u32 rh_cmn_clk__mco  ( u8 cmd);



#ifdef __cplusplus
}
#endif

#endif
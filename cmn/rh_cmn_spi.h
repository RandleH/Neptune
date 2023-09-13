/**
  ******************************************************************************
  * @file    rh_cmn_spi.h
  * @author  RandleH
  * @brief   This file contains code template.
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


#ifndef RH_CMN_SPI_H
#define RH_CMN_SPI_H


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"


#ifdef __cplusplus
extern "C"{
#endif

enum CmnSpiFreq{
  kCmnSpiFreq_48MHz  = 0,
  kCmnSpiFreq_16MHz  = 1,
  kCmnSpiFreq_1MHz   = 2,
  kCmnSpiFreq_2MHz   = 3,
  kCmnSpiFreq_4MHz   = 4
};


u32 rh_cmn_spi__init       ( enum CmnSpiFreq freq);
u32 rh_cmn_spi__send_block ( const u8 *buf, size_t nItems, size_t nTimes, u32 interval_delay_ms, u8* pDone);
u32 rh_cmn_spi__send_dma   ( const u8 *buf, size_t len, u8* pDone);

u32 rh_cmn_spi__send_value_block( const u8 value, size_t nTimes, u8* pDone);

u32 rh_cmn_spi__deinit     ( void);




#ifdef __cplusplus
}
#endif



#endif
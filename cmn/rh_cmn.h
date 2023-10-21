
/**
  ******************************************************************************
  * @file    rh_cmn.h
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


/// ========================================================================================================
///  Common Group Architech
/// ----------------------+----------------+----------------------------------------------------------------
///  USART                | rh_cmn_usart   | Module: Universal Sync/Async Reveiver Transmitter
/// ----------------------+----------------+----------------------------------------------------------------
///  SPI                  | rh_cmn_spi     | Module: Serial Peripheral Interface
/// ----------------------+----------------+----------------------------------------------------------------
///  MEM                  | rh_cmn_mem     | Module: Memory Management
/// ----------------------+----------------+----------------------------------------------------------------
///  ISR                  | rh_cmn_isr     | Module: Interrupt Service Routine
/// ----------------------+----------------+----------------------------------------------------------------
///  GPIO                 | rh_cmn_gpio    | Module: General Purpose IO
/// ----------------------+----------------+----------------------------------------------------------------
///  DELAY                | rh_cmn_delay   | Module: Delay 
/// ----------------------+----------------+----------------------------------------------------------------
///  CLK                  | rh_cmn_clk     | Module: Clock 
/// --------------------------------------------------------------------------------------------------------

/// ============================
/// ARM7       | ARM v4
/// -----------+----------------
/// ARM9       | ARM v5
/// -----------+----------------
/// ARM11      | ARM v6
/// -----------+----------------
/// Cortex-A   | ARM v7-A
/// -----------+----------------
/// Cortex-R   | ARM v7-R
/// -----------+----------------
/// Cortex-M   | ARM v7-M
/// ----------------------------

#ifndef RH_CMN_H
#define RH_CMN_H 



/* Exported types ------------------------------------------------------------*/
enum CmnConst{
    kCmnConst__USART_BUFFER_SIZE_POW_2   = 6      , /*!< Recommanded value: 5~7 */ 
    kCmnConst__USART_RX_IRQ_PRIORITY     = 7      ,\
    kCmnConst__USART_TX_IRQ_PRIORITY     = 7      ,\
    kCmnConst__USART_BAUDRATE            = 115200 ,\
    kCmnConst__SPI2_RX_IRQ_PRIORITY      = 5      ,\
    kCmnConst__SPI2_TX_IRQ_PRIORITY      = 5      
};


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"
#include "rh_cmn_clk.h"
#include "rh_cmn_usart.h"
#include "rh_cmn_delay.h"
#include "rh_cmn_mem.h"
#include "rh_cmn_spi.h"
#include "rh_cmn_gpio.h"
#include "rh_cmn_assert.h"
#include "rh_cmn_chip.h"
#include "rh_cmn_math.h"
#include "rh_cmn_rtc.h"
#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
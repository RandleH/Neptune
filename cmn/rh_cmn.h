
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



#include "rh_common.h"



typedef struct{
    volatile u64 clockTick;
}CmnData;
extern CmnData  gCommonData;



#include "rh_cmn_clk.h"
#include "rh_cmn_usart.h"
#include "rh_cmn_delay.h"
#include "rh_cmn_mem.h"
#include "rh_cmn_spi.h"
#include "rh_cmn_gpio.h"


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
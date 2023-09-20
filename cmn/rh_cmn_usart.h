/**
  ******************************************************************************
  * @file    rh_cmn_usart.h
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

#ifndef RH_CMN_USART_H
#define RH_CMN_USART_H

/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_usart.h"

#ifdef __cplusplus
extern "C"{
#endif


/* Exported types ------------------------------------------------------------*/
typedef struct CmnUsart{
    TaskHandle_t         task_handle_dma_tx_mgr;
    TaskHandle_t         task_handle_blk;
    USART_HandleTypeDef  hw_handle;
}CmnUsart_t;

extern CmnUsart_t g_CmnUsart;


u32 rh_cmn_usart__init( u32 baudrate);
u32 rh_cmn_usart__send_dma( const u8 *buf, size_t nItems, u8* pDone);
u32 rh_cmn_usart__send_blk( const u8 *buf, size_t nItems, u8* pDone);


int rh_cmn_usart__printf( char const *fmt, ...);

#ifdef __cplusplus
}
#endif



#endif
/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
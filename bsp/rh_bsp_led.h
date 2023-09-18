/**
  ******************************************************************************
  * @file    rh_bsp_led.h
  * @author  RandleH
  * @brief   BSP Led Driver.
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

#ifndef RH_BSP_LED_H
#define RH_BSP_LED_H


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"



/* Exported macro ------------------------------------------------------------*/
#define M_BSP_LED__R            (1<<0)
#define M_BSP_LED__G            (1<<1)
#define M_BSP_LED__B            (1<<2)
#define M_BSP_LED__ALL          ((M_BSP_LED__R)|(M_BSP_LED__G)|(M_BSP_LED__B))


/* Exported types ------------------------------------------------------------*/
typedef struct BspLed* pBspLed;


/* Exported functions --------------------------------------------------------*/
pBspLed rh_bsp_led__init  ( void);
u32     rh_bsp_led__switch( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay, u8 cmd);
u32     rh_bsp_led__toggle( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay);

u32     rh_bsp_led__state ( pBspLed pContext, u32 M_BSP_LED__xxxx);


u32     rh_bsp_led__deinit( pBspLed pContext);
#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
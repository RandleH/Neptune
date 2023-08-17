/**
  ******************************************************************************
  * @file    rh_bsp_key.h
  * @author  RandleH
  * @brief   This file contains source code for external key.
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

#ifndef RH_BSP_KEY_H
#define RH_BSP_KEY_H



/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"


/* Exported functions --------------------------------------------------------*/
#define M_BSP_KEY__U              ((u32)(1<<0))
#define M_BSP_KEY__D              ((u32)(1<<1))
#define M_BSP_KEY__L              ((u32)(1<<2))
#define M_BSP_KEY__R              ((u32)(1<<3))
#define M_BSP_KEY__A              ((u32)(1<<4))
#define M_BSP_KEY__B              ((u32)(1<<5))
#define M_BSP_KEY__C              ((u32)(1<<6))




#ifdef __cplusplus
extern "C"{
#endif



/* Exported types ------------------------------------------------------------*/
typedef struct BspKeyCfg_t{
  
  u32 raw;
}*pBspKeyCfg_t;

typedef struct BspKey* pBspKey;


/* Exported functions --------------------------------------------------------*/
pBspKey     rh_bsp_key__init( void);
u32         rh_bsp_key__read        ( pBspKey context, u32 M_BSP_KEY__xxxx);
u32         rh_bsp_key__trigger     ( pBspKey context, u32 M_BSP_KEY__xxxx, void (*func)( void*), void* param);
u32         rh_bsp_key__activate    ( pBspKey context, u32 M_BSP_KEY__xxxx);
u32         rh_bsp_key__deactivate  ( pBspKey context, u32 M_BSP_KEY__xxxx);
void        rh_bsp_key__deinit      ( pBspKey context);


#ifdef __cplusplus
}
#endif


#endif
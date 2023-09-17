/**
  ******************************************************************************
  * @file    rh_bsp_screen.h
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

#ifndef RH_BSP_SCREEN_H
#define RH_BSP_SCREEN_H


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"


/* Exported macro ------------------------------------------------------------*/
#define CFG_GRAM_SIZE               (240*10)


/* Exported types ------------------------------------------------------------*/
typedef u16   BspScreenPixel_t;



/* Functions -----------------------------------------------------------------*/
u32 rh_bsp_screen__init( void);
u32 rh_bsp_screen__set_dma_transfer( void);
u32 rh_bsp_screen__set_blk_transfer( void);

u32 rh_bsp_screen__flush( const BspScreenPixel_t *buf, u16 xs, u16 ys, u16 xe, u16 ye);





#endif
/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
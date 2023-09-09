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

/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"


#ifdef __cplusplus
extern "C"{
#endif



u32 rh_cmn_usart__init( u32 baudrate);
int rh_cmn_usart__printf( char const *fmt, ...);

#ifdef __cplusplus
}
#endif
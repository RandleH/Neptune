/**
  ******************************************************************************
  * @file    rh_cmn_chip.h
  * @author  RandleH
  * @brief   Chip information.
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

#ifndef RH_CMN_CHIP_H
#define RH_CMN_CHIP_H

/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"
#include "rh_cmn.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C"{
#endif


typedef union CmnChipUID{
    u32 dword[3];
    struct{
        u16 X;
        u16 y;
        u8  WAF;
        u8  LOT[7];
    }val;

    u8 raw[12];
}CmnChipUID_t;



#if 1
#define rh_cmn_chip__uid_31_00()          (*(u32*)UID_BASE)
#define rh_cmn_chip__uid_63_32()          (*((u32*)UID_BASE+0x00000004))
#define rh_cmn_chip__uid_95_64()          (*((u32*)UID_BASE+0x00000008))

CmnChipUID_t* rh_cmn_chip__uid( CmnChipUID_t *result);

#else
u32 rh_cmn_chip__uid_31_00( void);
u32 rh_cmn_chip__uid_63_32( void);
u32 rh_cmn_chip__uid_95_64( void);
#endif


#ifdef __cplusplus
}
#endif

#endif
/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
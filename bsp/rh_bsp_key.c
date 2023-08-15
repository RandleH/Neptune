/**
  ******************************************************************************
  * @file    rh_bsp_key.c
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


/* Includes ------------------------------------------------------------------*/
#include "rh_bsp_key.h"
#include "rh_cmn_gpio.h"




/// IO Connection
/// ====================================================== 
///  Key Pin IO           | STM32 Pin IO | Periphral
/// ----------------------+--------------+----------------
///  A     [GND=ON]       | PA8          | GPIO
/// ----------------------+--------------+----------------
///  B     [GND=ON]       | PA15         | GPIO
/// ----------------------+--------------+----------------
///  C     [GND=ON]       | PA1          | GPIO
/// ------------------------------------------------------




/* Private define ------------------------------------------------------------*/
#define M_GB_KEY__U              GPIOx    // N/A
#define M_GB_KEY__D              GPIOx    // N/A
#define M_GB_KEY__L              GPIOx    // N/A
#define M_GB_KEY__R              GPIOx    // N/A
#define M_GB_KEY__A              GPIOA
#define M_GB_KEY__B              GPIOA
#define M_GB_KEY__C              GPIOA

#define M_IO_KEY__U              ((u32)(0<<0))    // N/A
#define M_IO_KEY__D              ((u32)(0<<1))    // N/A
#define M_IO_KEY__L              ((u32)(0<<2))    // N/A
#define M_IO_KEY__R              ((u32)(0<<3))    // N/A
#define M_IO_KEY__A              ((u32)(1<<8))
#define M_IO_KEY__B              ((u32)(1<<15))
#define M_IO_KEY__C              ((u32)(1<<1))




#ifdef __cplusplus
extern "C"{
#endif


/* Private typedef -----------------------------------------------------------*/
typedef struct BspKey_t{
  u32 raw;
}BspKey_t;









/* Functions -----------------------------------------------------------------*/

/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
pBspKey_t rh_bsp_key__init( void){
  return NULL;
}





/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32 rh_bsp_key__read( pBspKey_t pState, u32 M_BSP_KEY__xxxx){
  u32 res = 0;
  if( (M_BSP_KEY__xxxx & M_BSP_KEY__A) && (0!=rh_cmn_gpio__readBit( M_GB_KEY__A, M_IO_KEY__A)) )
    res |= M_BSP_KEY__A;
  
  if( (M_BSP_KEY__xxxx & M_BSP_KEY__B) && (0!=rh_cmn_gpio__readBit( M_GB_KEY__B, M_IO_KEY__B)) )
    res |= M_BSP_KEY__B;

  if( (M_BSP_KEY__xxxx & M_BSP_KEY__C) && (0!=rh_cmn_gpio__readBit( M_GB_KEY__C, M_IO_KEY__C)) )
    res |= M_BSP_KEY__C;

  return res;
}




/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32         rh_bsp_key__trigger     ( pBspKey_t context, u32 M_BSP_KEY__xxxx, void (*func)( void*), void* param){
  return -1;
}

/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32         rh_bsp_key__activate    ( pBspKey_t context, u32 M_BSP_KEY__xxxx){
  return -1;
}

/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32         rh_bsp_key__deactivate  ( pBspKey_t context, u32 M_BSP_KEY__xxxx){
  return -1;
}

/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
void        rh_bsp_key__deinit      ( pBspKey_t context){
  
}


#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
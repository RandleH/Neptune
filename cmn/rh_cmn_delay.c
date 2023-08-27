/**
  ******************************************************************************
  * @file    rh_cmn_delay.c
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

/* C_FILE --------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"{
#endif


/* Includes ------------------------------------------------------------------*/
#include "rh_cmn_delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Functions -----------------------------------------------------------------*/
/** @defgroup Common 
 *  Project Common Group
 *  @{
 */

/** @defgroup DELAY
 *  @ingroup Common
 *  Project Delay Module
 *  @{
 */




/**
 * @brief       This function will stop the program for the given time range
 * @param       ms  milliseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_ms__halt( u32 ms){
    extern u32 SystemCoreClock;

    asm volatile(
        "mov    r1, %0                  \n" // Cycle: 1
        "mov    r2, #10000              \n" // Cycle: 1
        "udiv   r3, r1, r2              \n" // Cycle: 2-12
        "mul    r4, r0, r3              \n" // Cycle: 1
        "LOOP:                          \n"
        "sub    r4, r4, #1              \n" // Cycle: 1
        "cmp    r4, #0                  \n" // Cycle: 1
        "bne    LOOP                    \n" // Cycle: 1 + P
        : // No output
        : "r"(SystemCoreClock)
    );

    return 0;
}

/**
 * @brief       This function will stop the program for the given time range
 * @param       us  microseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_us__halt( u32 us){
    return 0;
}

/**
 * @brief       This function will suspend the current thread and will return by the given time range
 * @param       ms  milliseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_ms__escape( u32 ms){
    #warning "TODO"
    return 0;
}

/**
 * @brief       This function will suspend the current thread and will return by the given time range
 * @param       us  microseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_us__escape( u32 us){
    return 0;
}

/**
 * @brief       A callback will be executed after the given time. This function will NOT block any thread and will 
 *              return immediately
 * @param       ms  microseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_ms__callback( u32 ms, void(*func)(void*), void* param){
    return 0;
}

/**
 * @brief       A callback will be executed after the given time. This function will NOT block any thread and will 
 *              return immediately
 * @param       us  microseconds
 * @retval      Return 0 if no error occurs
*/
u32 rh_cmn_delay_us__callback( u32 us, void(*func)(void*), void* param){
    return 0;
}




/** @} */ // end of DELAY
/** @} */ // end of Common


#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
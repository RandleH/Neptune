
/**
  ******************************************************************************
  * @file    rh_cmn_usart.c
  * @author  RandleH
  * @brief   USART driver source code.
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


/// ====================================================== 
/// Device: CN480661 FT232RL
/// ====================================================== 
///  UART Pin IO          | STM32 Pin IO | Periphral
/// ----------------------+--------------+----------------
///  TX                   | PA2          | UART2
/// ----------------------+--------------+----------------
///  RX                   | PA3          | UART2
/// ------------------------------------------------------


/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "rh_cmn_usart.h"


/* Private define ------------------------------------------------------------*/
#define USARTx                  USART2
#define GPIOx_TX                GPIOA
#define GPIOx_RX                GPIOA
#define PIN_TX                  2       // Port A
#define PIN_RX                  3       // Port A
#define USART_CLK_INIT()        \
    do{\
        __USART2_CLK_ENABLE();\
    }while(0)





/* Functions -----------------------------------------------------------------*/
/** @defgroup Common 
 *  Project Common Group
 *  @{
 */

/** @defgroup USART
 *  @ingroup Common
 *  Project Universal Serial Async Recv-Transmitter
 *  @{
 */


/**
 * @brief       Send bytes through usart. This function will block the cpu resource until transfer finished.
 * @param       buf Pointer to the buffer
 * @param       len Length of the buffer in bytes
*/
static void send_bytes__blocking( const char *buf, size_t len){
    while(len--){
        while(1){
            if( USARTx->SR & USART_SR_TXE){
                USARTx->DR = *buf++;
                break;
            }
        }
    }
}


/**
 * @brief       Initialize the usart module
 * @param       baudrate    Typical values are: `9600`, `115200`
 * @retval      Return 0 if success
 *              Return 1 if config error occurs
*/
u32 rh_cmn_usart__init( u32 baudrate){
    USART_CLK_INIT();

    UART_HandleTypeDef s_UARTHandle;
    s_UARTHandle.Instance        = USARTx;
    s_UARTHandle.Init.BaudRate   = baudrate;
    s_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
    s_UARTHandle.Init.StopBits   = UART_STOPBITS_1;
    s_UARTHandle.Init.Parity     = UART_PARITY_NONE;
    s_UARTHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    s_UARTHandle.Init.Mode       = UART_MODE_TX_RX;
    s_UARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&s_UARTHandle) != HAL_OK){
        return 1;
    }

    return 0;
}


/**
 * @brief       Usart printf function.
 * @param       fmt  Formatted string with `va_list`
 * @retval      If success, return 0.
 *              Return 1 if buffer overflow.
 *              Return 2 if communication time out.
*/
int rh_cmn_usart__printf( char const *fmt, ...){
    static char tmp[256] = {0};

    va_list ap;
    va_start( ap, fmt);
    int len = vsnprintf( tmp, 256U, fmt, ap);
    va_end(ap);

    if( len==-1 )
        return 1;

    send_bytes__blocking( tmp, len);

    return 0;
}










/** @} */ // end of USART
/** @} */ // end of Common


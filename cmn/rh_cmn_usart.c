
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


#include "rh_cmn.h"



/* Private define ------------------------------------------------------------*/
#define USARTx                  USART2
#define GPIOx_TX                GPIOA
#define GPIOx_RX                GPIOA
#define PIN_TX                  2       // Port A
#define PIN_RX                  3       // Port A





/* Global variables ----------------------------------------------------------*/
CmnUsart_t g_CmnUsart = {0};
static DMA_HandleTypeDef husart2_dma_tx = {0};
static DMA_HandleTypeDef husart2_dma_rx = {0};




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




static void rh_cmn_usart__tx_error_callback( USART_HandleTypeDef * hw_handle){

}

static void rh_cmn_usart__tx_half_callback( USART_HandleTypeDef * hw_handle){
    
}

/**
 * @brief       This callback function ONLY run in ISR 
 * @return      (none)
*/
static void rh_cmn_usart__rx_completed_callback( USART_HandleTypeDef * hw_handle){
    if( hw_handle == &g_CmnUsart.hw_handle){

    }
}

/**
 * @brief       This callback function ONLY run in ISR 
 * @return      (none)
*/
static void rh_cmn_usart__tx_completed_callback( USART_HandleTypeDef * hw_handle){
    if( hw_handle == &g_CmnUsart.hw_handle){
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR( g_CmnUsart.task_handle_dma_tx_mgr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}



/**
 * @brief       Initialize the usart module
 * @param       baudrate    Typical values are: `9600`, `115200`
 * @retval      Return 0 if success
 *              Return 1 if DMA TX init error
 *              Return 2 if DMA RX init error
 *              Return 3 if USART init error
*/
u32 rh_cmn_usart__init( u32 baudrate){
    
    /* CLK Config */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA config */
    __HAL_LINKDMA( &g_CmnUsart.hw_handle, hdmatx, husart2_dma_tx);
    __HAL_LINKDMA( &g_CmnUsart.hw_handle, hdmarx, husart2_dma_rx);

    g_CmnUsart.hw_handle.hdmatx->Init.Channel             = DMA_CHANNEL_4;
    g_CmnUsart.hw_handle.hdmatx->Init.Direction           = DMA_MEMORY_TO_PERIPH;
    g_CmnUsart.hw_handle.hdmatx->Init.PeriphInc           = DMA_PINC_DISABLE;
    g_CmnUsart.hw_handle.hdmatx->Init.MemInc              = DMA_MINC_ENABLE;
    g_CmnUsart.hw_handle.hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_CmnUsart.hw_handle.hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    g_CmnUsart.hw_handle.hdmatx->Init.Mode                = DMA_NORMAL; 
    g_CmnUsart.hw_handle.hdmatx->Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    g_CmnUsart.hw_handle.hdmatx->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    g_CmnUsart.hw_handle.hdmatx->Init.MemBurst            = DMA_MBURST_SINGLE;
    g_CmnUsart.hw_handle.hdmatx->Init.PeriphBurst         = DMA_PBURST_SINGLE;
    g_CmnUsart.hw_handle.hdmatx->Instance                 = DMA1_Stream6;
    if( HAL_OK!=HAL_DMA_Init( g_CmnUsart.hw_handle.hdmatx)){
        return 1;
    }

    g_CmnUsart.hw_handle.hdmarx->Init.Channel             = DMA_CHANNEL_6;
    g_CmnUsart.hw_handle.hdmarx->Init.Direction           = DMA_PERIPH_TO_MEMORY;
    g_CmnUsart.hw_handle.hdmarx->Init.PeriphInc           = DMA_PINC_DISABLE;
    g_CmnUsart.hw_handle.hdmarx->Init.MemInc              = DMA_MINC_ENABLE;
    g_CmnUsart.hw_handle.hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_CmnUsart.hw_handle.hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    g_CmnUsart.hw_handle.hdmarx->Init.Mode                = DMA_NORMAL; 
    g_CmnUsart.hw_handle.hdmarx->Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    g_CmnUsart.hw_handle.hdmarx->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    g_CmnUsart.hw_handle.hdmarx->Init.MemBurst            = DMA_MBURST_SINGLE;
    g_CmnUsart.hw_handle.hdmarx->Init.PeriphBurst         = DMA_PBURST_SINGLE;
    g_CmnUsart.hw_handle.hdmarx->Instance                 = DMA1_Stream7;
    if( HAL_OK!=HAL_DMA_Init( g_CmnUsart.hw_handle.hdmarx)){
        return 2;
    }
    #warning "DMA unverified"

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, kCmnConst__USART_TX_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, kCmnConst__USART_RX_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    
    g_CmnUsart.hw_handle.Instance          = USARTx;
    g_CmnUsart.hw_handle.Init.BaudRate     = baudrate;
    g_CmnUsart.hw_handle.Init.WordLength   = UART_WORDLENGTH_8B;
    g_CmnUsart.hw_handle.Init.StopBits     = UART_STOPBITS_1;
    g_CmnUsart.hw_handle.Init.Parity       = UART_PARITY_NONE;
    g_CmnUsart.hw_handle.Init.Mode         = UART_MODE_TX_RX;
    g_CmnUsart.hw_handle.Init.CLKPolarity  = USART_POLARITY_LOW; 
    g_CmnUsart.hw_handle.Init.CLKPhase     = USART_PHASE_1EDGE;
    g_CmnUsart.hw_handle.Init.CLKLastBit   = USART_LASTBIT_DISABLE;
    
    if (HAL_USART_Init(&g_CmnUsart.hw_handle) != HAL_OK){
        return 3;
    }

    __HAL_USART_ENABLE_IT( &g_CmnUsart.hw_handle, UART_IT_TC);
    HAL_NVIC_SetPriority( USART2_IRQn, kCmnConst__USART_TX_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ( USART2_IRQn);

    /* Must set `USE_HAL_USART_REGISTER_CALLBACKS` to 1 in "cfg/stm32f4xx_hal_conf.h" */
    /* Must run after `HAL_USART_Init()` was called */
    g_CmnUsart.hw_handle.ErrorCallback      = rh_cmn_usart__tx_error_callback;
    g_CmnUsart.hw_handle.TxCpltCallback     = rh_cmn_usart__tx_completed_callback;
    g_CmnUsart.hw_handle.RxCpltCallback     = rh_cmn_usart__rx_completed_callback;
    g_CmnUsart.hw_handle.TxHalfCpltCallback = rh_cmn_usart__tx_half_callback;
    
    return 0;
}

u32 rh_cmn_usart__send_dma( const u8 *buf, size_t nItems, u8* pDone){
#warning "Unverified"
    if( nItems==0 || buf==NULL){
        return 1;
    }

    g_CmnUsart.task_handle_dma_tx_mgr = xTaskGetCurrentTaskHandle();

    HAL_USART_Transmit_DMA( &g_CmnUsart.hw_handle, (u8*)buf, nItems);
    
    /* Wait transmission completed */
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY);

    return 0;
}

/**
 * @brief       Send bytes through usart. This function will block the cpu resource until transfer finished.
 * @param       buf Pointer to the buffer
 * @param       len Length of the buffer in bytes
*/
u32 rh_cmn_usart__send_blk( const u8 *buf, size_t nItems, u8* pDone){
    while(nItems--){
        while(1){
            if( USARTx->SR & USART_SR_TXE){
                USARTx->DR = *buf++;
                break;
            }
        }
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

    rh_cmn_usart__send_blk( tmp, len, NULL);

    return 0;
}










/** @} */ // end of USART
/** @} */ // end of Common


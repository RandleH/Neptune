/**
  ******************************************************************************
  * @file    rh_cmn_spi.c
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



/// ========================================================= 
/// Device: GC9A01 240x240 TFT Screen
/// ========================================================= 
///  Screen Pin IO        | STM32 Pin IO    | Periphral
/// ----------------------+-----------------+----------------
///  SCL                  | PB13 (or PA5)   | SPI2
/// ----------------------+-----------------+----------------
///  SDA                  | PB15 (or PA7)   | SPI2
/// ----------------------+-----------------+----------------
///  DC                   | PC15            | GPIO
/// ----------------------+-----------------+----------------
///  RST                  | PB2             | GPIO
/// ----------------------+-----------------+----------------
///  CS                   | PC14            | GPIO
/// ---------------------------------------------------------

/// ========================================================= 
/// DMA Configuration 
/// @ref        RM0383, Page 170
/// ========================================================= 
///  DMA Base        | Periphral | Stream    | Channel
/// -----------------------------+-----------+---------------
///  DMA1            | SPI2_TX   | 4         | 0
/// ---------------------------------------------------------



/* Includes ------------------------------------------------------------------*/
#include "rh_cmn_spi.h"
#include "rh_cmn_clk.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_dma.h"



/* Private define ------------------------------------------------------------*/
#define SPIx                SPI2

#define DMAx_STRx           DMA1_Stream4
#define DMA_CHx             DMA_CHANNEL_0

#define PIN_SDA             GPIO_PIN_15  // Port B
#define PIN_SCL             GPIO_PIN_13  // Port B
#define PIN_DC              GPIO_PIN_6   // Port B
#define PIN_RST             GPIO_PIN_2   // Port B
#define PIN_CS              GPIO_PIN_7   // Port B

#define GPIOx_SDA           GPIOB
#define GPIOx_SCL           GPIOB
#define GPIOx_DC            GPIOB
#define GPIOx_RST           GPIOB
#define GPIOx_CS            GPIOB

#define WRITE_CS(x)         do{ x==0? CLEAR_BIT( GPIOx_CS->ODR , PIN_CS) : SET_BIT( GPIOx_CS->ODR , PIN_CS ); }while(0)
#define WRITE_DC(x)         do{ x==0? CLEAR_BIT( GPIOx_DC->ODR , PIN_DC ): SET_BIT( GPIOx_DC->ODR , PIN_DC ); }while(0)
#define WRITE_RST(x)        do{ x==0? CLEAR_BIT( GPIOx_RST->ODR, PIN_RST): SET_BIT( GPIOx_RST->ODR, PIN_RST); }while(0)

#define SPI_CLK_ENABLE()\
    do{\
        SET_BIT( RCC->APB1ENR, RCC_APB1ENR_SPI2EN);\
    }while(0)

#define SPI_CLK_DISABLE()\
    do{\
        CLEAR_BIT( RCC->APB1ENR, RCC_APB1ENR_SPI2EN);\
    }while(0)


#define GPIO_CLK_ENABLE()\
    do{\
        SET_BIT( RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
    }while(0)

#define GPIO_CLK_DISABLE()\
    do{\
        CLEAR_BIT( RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
    }while(0)    


#ifdef __cplusplus
extern "C"{
#endif




static SPI_HandleTypeDef hspi2     = {0};
static DMA_HandleTypeDef hspi2_dma = {0};

/* Functions -----------------------------------------------------------------*/
/** @defgroup Common 
 *  Project Common Group
 *  @{
 */

/** @defgroup SPI
 *  @ingroup Common
 *  Project Serial Peripheral Interface
 *  @{
 */



/**
 * @retval  Possible values: 0, 1, 2, 4, 6, 8, 16, 32
*/
static u8 rh_cmn_spi__calculate_div( enum CmnCpuFreq f_cpu, enum CmnSpiFreq f_spi){
    u8 F_cpu=0, F_spi=0;
    switch( f_cpu ){
        case kCmnCpuFreq_16MHz:{
            F_cpu=16;break;
        }
        case kCmnCpuFreq_25MHz:{
            F_cpu=25;break;
        }
        case kCmnCpuFreq_32MHz:{
            F_cpu=32;break;
        }
        case kCmnCpuFreq_96MHz:{
            F_cpu=96;break;
        }
        default:{
            break;
        }
    }
    switch( f_spi){
        case kCmnSpiFreq_1MHz:{
            F_spi=1; break;
        }
        case kCmnSpiFreq_2MHz:{
            F_spi=2; break;
        }
        case kCmnSpiFreq_4MHz:{
            F_spi=4; break;
        }
        case kCmnSpiFreq_16MHz:{
            F_spi=16; break;
        }
        case kCmnSpiFreq_48MHz:{
            F_spi=48; break;
        }
        default:{
            break;
        }
    }
    return (u8)(F_cpu/F_spi);
}



/**
 * @brief       Initialize SPI module. Internal connected to hardware
 * @param       freq    SPI frequency @ref enum CmnSpiFreq
 * @retval      Return 0 if success
 *              Return 1 if SPI initialize failed 
 *              Return 2 if DMA initialize failed
*/
u32 rh_cmn_spi__init( enum CmnSpiFreq freq){
    /* CLK Config */
    __SPI2_CLK_ENABLE();
    
    /* SPI config */
    hspi2.Instance               = SPIx;
    hspi2.Init.Mode              = SPI_MODE_MASTER;
    hspi2.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi2.Init.NSS               = SPI_NSS_SOFT;
    hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial     = 10;

    switch(rh_cmn_spi__calculate_div( rh_cmn_clk__get_cpu(), freq)){
        // f_AHB = f_cpu           ---> eg: 96MHz
        // f_APB = f_AHB / P1      ---> eg: 48MHz
        // f_spi = f_APB / P2      ---> eg: 
        default:
        case 2:{    
            hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;  // MUST set to `SPI_BAUDRATEPRESCALER_2`
            break;
        }
        case 4:{
            hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
            break;
        }
        case 8:{
            hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
            break;
        }
        case 16:{
            hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
            break;
        }
    }
    
    if( HAL_OK!=HAL_SPI_Init(&hspi2)){
        return 1;
    }

    /* DMA config */
    hspi2.hdmatx = &hspi2_dma;
    hspi2.hdmatx->Init.Channel             = DMA_CHANNEL_0;
    hspi2.hdmatx->Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hspi2.hdmatx->Init.PeriphInc           = DMA_PINC_DISABLE;
    hspi2.hdmatx->Init.MemInc              = DMA_MINC_ENABLE;
    hspi2.hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hspi2.hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hspi2.hdmatx->Init.Mode                = DMA_NORMAL; // ?
    hspi2.hdmatx->Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    hspi2.hdmatx->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hspi2.hdmatx->Init.MemBurst            = DMA_MBURST_SINGLE;
    hspi2.hdmatx->Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hspi2.hdmatx->Instance                 = DMAx_STRx;

    if( HAL_OK!=HAL_DMA_Init( &hspi2.hdmatx)){
        return 2;
    }

    #warning "Unverified"
    return 0;
}

/**
 * @brief       Send data. This function will block the program until transfer completed
 * @retval      Return 0 if success
 *              Return 1 if nothing to transmit
*/
u32 rh_cmn_spi__send_block( const u8 *buf, size_t len, u8* pDone){
    #warning "Unverified"
    if( pDone!=NULL){
        *pDone = false;
    }

    if( buf==NULL || len==0){
        return 1;
    }

    CLEAR_BIT( SPIx->CR1, SPI_CR1_DFF);
    SET_BIT( SPIx->CR1, SPI_CR1_BIDIOE);
    if( !READ_BIT( SPIx->CR1, SPI_CR1_SPE) ){
        SET_BIT( SPIx->CR1, SPI_CR1_SPE);
    }
    
    while( len--){
        SPIx->DR = *buf++;
        while( 0==READ_BIT( SPIx->SR, SPI_SR_TXE));  // Blocking function
    }
    
    *pDone = true;
    return 0;
}

/**
 * @brief       Send data using DMA channel.
 * @ref         RM0383, Page 580 -- SPI communication using DMA
 *              RM0383, Page 170 -- DMA Table
 * @param       tx_buf      Data to be sent
 * @param       len         Buffer length
 * @param       pDone       Pointer to the flag. Buffer should remain valid when pDone is FALSE.             
 * @retval      Return 0 if params are OK
 *              Return 1 if `pDone` is a NULL pointer
 *              Return 2 if `buf` or `len` is 0. Nothing to send
*/
u32 rh_cmn_spi__send_dma( const u8 *tx_buf, size_t len, u8* pDone){

    #warning "Unimplemented"
    return 0;
}

/**
 * @brief       Send data using DMA channel.
 * @param       tx_buf      Data to be sent
 * @param       len         Buffer length
 * @param       pDone       Pointer to the flag. Buffer should remain valid when pDone is FALSE.             
 * @retval      Return 0 if params are OK
 *              Return 1 if `pDone` is a NULL pointer
 *              Return 2 if `buf` or `len` is 0. Nothing to send
*/
u32 rh_cmn_spi__recv_dma( u8 *rx_buf, size_t len, u8* pDone){
    #warning "Unimplemented"
    return 0;
}


/**
 * @brief
 * @ref         RM0383, Page 579
 * @retval      Always return 0
*/
u32 rh_cmn_spi__deinit( void){
    // Reference Manual, RM0383, Page 579
    SPIx->CR1 = 0x0000;
    SPIx->CR2 = 0x0000;
    SPI_CLK_DISABLE();
    return 0;
}







/** @} */ // end of USART
/** @} */ // end of Common



#ifdef __cplusplus
}
#endif


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
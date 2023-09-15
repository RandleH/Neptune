/**
  ******************************************************************************
  * @file    rh_cmn_gpio.c
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


/// ========================================================================== 
/// GPIO 
/// ========================================================================== 
///  STM32 Pin IO    | AlterFunc  | PULL  | MODE     | Description
/// -----------------+------------+-------+-----------------------------------
///  PA2             | USART2     | Up    | AFPP     | USART TX (stm32)
/// -----------------+------------+-------+----------+------------------------
///  PA3             | USART2     | Up    | AFPP     | USART RX (stm32)
/// -----------------+------------+-------+----------+------------------------
///  PB13 (or PA5)   | SPI2       | Up    | AFPP     | Screen Spi Sclk
/// -----------------+------------+-------+----------+------------------------
///  PB15 (or PA7)   | SPI2       | Up    | AFPP     | Screen Spi Sda
/// -----------------+------------+-------+----------+------------------------
///  PB2             | GPIO-OUT   | Up    | PP       | Screen Reset pin
/// -----------------+------------+-------+----------+------------------------
///  PC14            | GPIO_OUT   | N     | PP       | Screen CS pin
/// -----------------+------------+------------------+------------------------
///  PC15            | GPIO-OUT   | Up    | PP       | Screen DC pin  
/// -----------------+------------+-------+----------+------------------------
///  PC13            | GPIO-OUT   | N     | PP       | LED Green
/// -----------------+------------+-------------------------------------------
/// --------------------------------------------------------------------------




/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rh_cmn_gpio.h"


/* Private define ------------------------------------------------------------*/


/**
 * @brief       GPIO Initialization
 * @details     A2,A3    | AFPP | USART2 | PULLUP
 *              B2       | PP   |        | PULLUP
 *              B13,B15  | AFPP | SPI2   | PULLUP
 *              C13      | PP   |        | PULLUP
 *              C14,C14  | PP   |        | PULLUP
 * @return      Always return 0
*/
u32 rh_cmn_gpio__init( void){
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    __GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin       = ((1<<2)|(1<<3));
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = GPIO_AF7_USART2;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.Pin       = (1<<8);
    GPIO_InitStructure.Alternate = GPIO_AF0_MCO;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStructure);
    

    __GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin       = (1<<2);
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Alternate = 0x00;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = (1<<13)|(1<<15);
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure);
    
    __GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin       = (1<<9);
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = GPIO_AF0_MCO;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = ((1<<13));
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Alternate = 0x00;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_MEDIUM;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = ((1<<14)|(1<<15));
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Alternate = 0x00;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStructure);
    
    /* Disable MCO frequency output */
    rh_cmn_clk__mco_disable();
    
    return 0;
}

u32 inline rh_cmn_gpio__readAll( void* GPIOx){
    return ((GPIO_TypeDef *)GPIOx)->IDR;
}



u32 inline rh_cmn_gpio__readBit( void* GPIOx, u32 pos){
    return ((((GPIO_TypeDef *)GPIOx)->IDR) & pos);
}


u32 inline rh_cmn_gpio__setBit( void* GPIOx, u32 pos){
    (((GPIO_TypeDef *)GPIOx)->BSRR) |= pos;
    return 0;
}

u32 inline rh_cmn_gpio__unsetBit( void* GPIOx, u32 pos){
    (((GPIO_TypeDef *)GPIOx)->BSRR) |= (u32)(pos<<16);
    return 0;
}
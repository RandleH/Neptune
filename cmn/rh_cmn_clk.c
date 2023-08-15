/**
  ******************************************************************************
  * @file    rh_cmn_clk.c
  * @author  RandleH
  * @brief   Configuration for CPU clock speed.
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
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

#include "rh_cmn_clk.h"




#ifdef __cplusplus
extern "C"{
#endif


 /* Private functions ---------------------------------------------------------*/
/**
 * @addtogroup      Common
 * 
 * @brief
 * @return
*/
u32 rh_cmn_clk__cpu  ( enum CmnCpuFreq frequency ){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  
  switch (frequency){
    case _16MHz:{
      
      break;
    }

    case _25MHz:{
      
      break;
    }

    case _32MHz:{
      RCC_OscInitStruct.PLL.PLLM = 25;
      RCC_OscInitStruct.PLL.PLLN = 192;
      RCC_OscInitStruct.PLL.PLLP = 6;
      RCC_OscInitStruct.PLL.PLLQ = 4;

      break;
    }

    case _48MHz:{
      RCC_OscInitStruct.PLL.PLLM = 25;
      RCC_OscInitStruct.PLL.PLLN = 192;
      RCC_OscInitStruct.PLL.PLLP = 4;
      RCC_OscInitStruct.PLL.PLLQ = 4;
      break;
    }

    case _96MHz:{
      RCC_OscInitStruct.PLL.PLLM = 25;
      RCC_OscInitStruct.PLL.PLLN = 192;
      RCC_OscInitStruct.PLL.PLLP = 2;
      RCC_OscInitStruct.PLL.PLLQ = 4;
      break;
    }

    default:
      return -1;
  }

  if( frequency==_96MHz || frequency==_48MHz || frequency==_32MHz ){
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  }else if( frequency==_16MHz){
    
  }

  HAL_RCC_OscConfig(&RCC_OscInitStruct); //  HAL_OK;

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_3); // HAL_OK;

  HAL_RCC_EnableCSS();
  return 0;
}


/**
 * @addtogroup      Common
 * 
 * @brief
 * @return
*/
u32 rh_cmn_clk__rtc  ( void){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct); //   HAL_OK;
  __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
  __HAL_RCC_RTC_ENABLE();
  return 0;
}


/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32 rh_cmn_clk__reset( void){
  SCB->CPACR   |= ((3UL << 10*2)|(3UL << 11*2));
  RCC->CR      |= (uint32_t)0x00000001;
  RCC->CFGR     = 0x00000000;
  RCC->CR      &= (uint32_t)0xFEF6FFFF;
  RCC->PLLCFGR  = 0x24003010;
  RCC->CR      &= (uint32_t)0xFFFBFFFF;
  RCC->CIR      = 0x00000000;

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR      |= PWR_CR_VOS;
}


/**
 * @addtogroup
 * 
 * @brief
 * @return
*/
u32 rh_cmn_clk__mco  ( u8 cmd){

}


#ifdef __cplusplus
}
#endif
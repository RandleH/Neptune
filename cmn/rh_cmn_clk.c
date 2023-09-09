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
#include "rh_cmn_mem.h"



#ifdef __cplusplus
extern "C"{
#endif




/* Private typedef -----------------------------------------------------------*/
struct CmnClkEpoch{
    u32   tick;
};
typedef struct CmnClkEpoch CmnClkEpoch_t;



struct CmnClk{
    CmnClkEpoch_t *pHead;
    u32 tick;
};
typedef struct CmnClk CmnClk_t;
typedef struct CmnClk *pCmnClk;
static CmnClk_t s_context = {0};



/* Private functions ---------------------------------------------------------*/
/** @defgroup Common 
 *  Project Common Group
 *  @{
 */



/**
 * @brief   Set CPU frequency globally
 *          96MHz is a typical value for normal operation
 *          16MHz is used when operating in low power mode
 *          HSE:    96MHz/48MHz/32MHz 
 *          HSI:    16MHz
 * @param   frequency   @ref enum CmnCpuFreq
 * @return  Return 0 when success
 *          Return 1 when internal failure occured
 *          Return 2 when unsupported frequency is provided
*/
u32 rh_cmn_clk__set_cpu  ( enum CmnCpuFreq frequency ){
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
        case kCmnCpuFreq_16MHz:{
            SystemCoreClock = 16000000U;
        break;
        }

        case kCmnCpuFreq_25MHz:{
        
        break;
        }

        case kCmnCpuFreq_32MHz:{
            SystemCoreClock = 32000000U;
            RCC_OscInitStruct.PLL.PLLM = 25;
            RCC_OscInitStruct.PLL.PLLN = 192;
            RCC_OscInitStruct.PLL.PLLP = 6;
            RCC_OscInitStruct.PLL.PLLQ = 4;
            break;
        }

        case kCmnCpuFreq_96MHz:{
            SystemCoreClock = 96000000U;
            RCC_OscInitStruct.PLL.PLLM = 25;
            RCC_OscInitStruct.PLL.PLLN = 192;
            RCC_OscInitStruct.PLL.PLLP = 2;
            RCC_OscInitStruct.PLL.PLLQ = 4;
            break;
        }

        default:{
            SystemCoreClock = 0U;
            return -1;
        }
    }
    
    /* Ref: https://github.com/kowalski100/STM32F4-HAL-Examples/blob/master/Clock%20Sources/main.c */
    if( frequency==kCmnCpuFreq_96MHz || frequency==kCmnCpuFreq_32MHz ){
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
        RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    }else if( frequency==kCmnCpuFreq_16MHz){
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
        RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
        RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
        RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    }

    HAL_RCC_OscConfig(&RCC_OscInitStruct); //  HAL_OK;

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; 
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_3); // HAL_OK;

    return 0;
}



enum CmnCpuFreq rh_cmn_clk__get_cpu  ( void){
    switch( HAL_RCC_GetSysClockFreq() ){
        case 16000000:
            return kCmnCpuFreq_16MHz;
        case 32000000:
            return kCmnCpuFreq_32MHz;    
        case 96000000:
            return kCmnCpuFreq_96MHz;
        case 25000000:
            return kCmnCpuFreq_25MHz;
        default:
            break;    
    }
    //...//
    return kCmnCpuFreq_END;
}


/**
 * @brief   Set RTC frequency globally
 *          32.768KHz is provided through hardware
 *          LSE:    32.768KHz
 * @return  Return 0 when success
 *          Return 1 when internal failure occured
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
 * @brief       Set CPU & RTC frequency globally
 *              Default Config:  CPU-HSI-16MHz   RTC-Disabled
 * @return      Always return 0
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

    return 0;
}


/** 
 * @brief       Export CPU frequency signal to pin
 * @param       cmd 1=enable; 0=disable
 *              f_io = f_cpu/4
 * @return      Return 0 if success
*/
u32 rh_cmn_clk__mco  ( u8 cmd){
    return 0;
}


/**
 * @brief       Enable the systick interrupt service
 * @param       frequency   Systick interrupt service frequency
 *              Can be any member of @ref enum CmnSystickFreq
 * @return      Return 0 if success
 *              Return 1 if config error occurs
 *              Return -1 if wrong param given
*/
u32 rh_cmn_clk__systick_enable( enum CmnSystickFreq frequency){
    switch( frequency){
        case kCmnSystickFreq_1KHz:
            if(HAL_OK!=HAL_SetTickFreq(HAL_TICK_FREQ_1KHZ))
                return 1;
            break;
        case kCmnSystickFreq_100Hz:
            if(HAL_OK!=HAL_SetTickFreq(HAL_TICK_FREQ_100HZ))
                return 1;
            break;
        case kCmnSystickFreq_10Hz:
            if(HAL_OK!=HAL_SetTickFreq(HAL_TICK_FREQ_10HZ))
                return 1;
            break;
        default:
            return -1;
    }
    return 0;
}

/**
 * @brief       Disable the systick interrupt service
 * @return      Return 0 if success
*/
u32 rh_cmn_clk__systick_disable( void){
    return 0;
}

void rh_cmn_clk__systick_task( void* param){
    if( s_context.tick==UINT32_MAX){
        s_context.tick = 0;
    }else{
        ++s_context.tick;
    }
}

/**
 * @brief       Create a time epoch on systick
 * @return      Return a service handle. If NULL, no memory available
*/
pCmnClkEpoch rh_cmn_clk__systick_create_epoch( void){
    pCmnClkEpoch pEpoch = (pCmnClkEpoch)rh_cmn_mem__malloc( sizeof(CmnClkEpoch_t));

    if( pEpoch==NULL){
        return NULL;
    }

    pEpoch->tick     = rh_cmn_clk__systick_now();    
    return pEpoch;
}


/**
 * @brief       The maximum duration is (0xffffffff * systick_period). Exceed this will result an undefined behavior.
 *              Maximum duration:  1193 hours when   1ms systick period
 *                                   13 years when 100ms systick period
 *                                  136 years when   1s  systick period
 * @param       pEpoch  Epoch handler returned by `rh_cmn_clk__systick_create_epoch()` function
 * @param       update  Update the epoch therefore check point will set to now.
 * @return      Return  duration since last epoch.
*/
u32 rh_cmn_clk__systick_duration_epoch( pCmnClkEpoch pEpoch, bool update){
    u32 duration = 0;
    if( pEpoch->tick < s_context.tick){
        duration = s_context.tick = pEpoch->tick;
        if( update==true){
            pEpoch->tick = s_context.tick;
        }
    }else{
        duration = UINT32_MAX - pEpoch->tick + s_context.tick;
    }
    return duration;
}

u32 rh_cmn_clk__systick_update_epoch( pCmnClkEpoch pEpoch){
    if( pEpoch==NULL) 
        return UINT32_MAX;
    pEpoch->tick = s_context.tick;
    return 0;
}

void rh_cmn_clk__systick_delete_epoch( pCmnClkEpoch pEpoch){
    rh_cmn_mem__free( pEpoch);
}



u32 inline rh_cmn_clk__systick_now( void){
    return s_context.tick;
}



/** @} */ // end of Common

#ifdef __cplusplus
}
#endif
/**
  ******************************************************************************
  * @file    rh_watch.h
  * @author  RandleH
  * @brief   Top source code for Watch OS
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

#ifndef RH_WATCH_H
#define RH_WATCH_H

/* Includes ------------------------------------------------------------------*/
#include "rh_app.h"
#include "rh_cmn.h"
#include "rh_bsp.h"

#ifdef __cplusplus
extern "C"{
#endif    


/* Exported types ------------------------------------------------------------*/
typedef struct WatchTopStructure{

    struct AppList{
        AppClock_t      *clock;

    }app;

    struct SysList{
        AppTrace_t      *logger;            /*!< System Application: Logger Application */
        AppTask_t       *taskmgr;           /*!< System Application: Task Manager Application */
        AppGui_t        *gui;               /*!< System Application: Graph User Interface */
    }sys;

    struct HwList{
        CmnSpi_t        *spi;               /*!< Hardware Application: SPI interface @ref `rh_cmn_spi.h` */
        CmnRtc_t        *rtc;               /*!< Hardware Application: RTC interface @ref `rh_cmn_rtc.h` */
        CmnUsart_t      *usart;
        BspScreen_t     *screen;
    }hw;
    
    void (*entrance)( void*);

}WatchTopStructure_t;


/* Exported variable ---------------------------------------------------------*/
extern const WatchTopStructure_t watch;


#ifdef __cplusplus
}
#endif    

#endif  /* End of `RH_WATCH_H` */

/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
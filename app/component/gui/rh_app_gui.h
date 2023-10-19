/**
  ******************************************************************************
  * @file    rh_app_gui.h
  * @author  RandleH
  * @brief   Application for GUI
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


#ifndef RH_APP_GUI_H
#define RH_APP_GUI_H


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"


/**
 * @brief       Application Super Structure
 * @note        Globally created in program
 *              size: ? bytes
 * @attention   Do NOT change any value. Use api function instead
 *              Do NOT define in any functions.
*/
typedef struct AppGui{
    /* Private -----------------------------------------------------------*/
    TaskHandle_t       screen_owner;    /* !< Task that take ctrl of the screen */
    TaskHandle_t       task_refreash;


    lv_color_t         gram[kAppConst__GUI_NUM_OF_GRAM][kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM];


#if   (LVGL_VERSION_MAJOR==8) && ((LVGL_VERSION_MINOR==3) || (LVGL_VERSION_MINOR==2))
    lv_disp_t         *display;
    lv_disp_drv_t      driver;
    lv_disp_draw_buf_t buf;
#elif (LVGL_VERSION_MAJOR==9) && (LVGL_VERSION_MINOR==0) && (LVGL_VERSION_PATCH==0)
    lv_disp_t         *display;
#else
  #error "Unknown LVGL version."
#endif
    
    bool               isInitialized;
    
    /* Public ------------------------------------------------------------*/
    AppTaskUnit_t   *launch_list;           /*!< To launch app, add these through `AppTaskMgr` */
    size_t           launch_list_len;

    AppTaskUnit_t   *run_list;              /*!< To run app, add these through `AppTaskMgr` */
    size_t           run_list_len;

    
    int  (*launch)( void);
    void (*yeild)( TaskHandle_t from_whom);
    int  (*request)( TaskHandle_t from_whom );
    void (*halt)( void);
    
}AppGui_t;



extern AppGui_t g_AppGui;

#endif


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
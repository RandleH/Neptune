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
    TaskHandle_t    task_refreash;  /* !< Task to refreash screen */
    
    TaskHandle_t    task_master;    /* !< Task that take ctrl of the screen */
    
    lv_color_t         gram[kAppConst__GUI_NUM_OF_GRAM][kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM];
    lv_disp_t         *display;
    

    
    /* Public ------------------------------------------------------------*/
    int  (*launch)( void);
    void (*yeild)( TaskHandle_t from_whom);
    int  (*request)( TaskHandle_t from_whom );
    void (*halt)( void);
    
}AppGui_t;



extern AppGui_t g_AppGui;

#endif


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
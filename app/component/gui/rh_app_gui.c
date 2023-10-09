/**
  ******************************************************************************
  * @file    rh_app_gui.c
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



/* Includes ------------------------------------------------------------------*/
#include "rh_app.h"
#include "rh_bsp.h"
#include "lvgl.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define self                                (&g_AppGui)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void task_func__refreash( void* param){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = kAppConst__GUI_FRAME_RATE_MS*1000/1000;

    xLastWakeTime = xTaskGetTickCount();

    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        lv_tick_inc(xFrequency);
        lv_timer_handler();
    }
    
}

static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * buf){
    
}


/* Functions -----------------------------------------------------------------*/
static int launch_function( void){
    u32 res = 0x00000000;
    
    /* Initialize LVGL Library */
    lv_init();
    
    
    __asm("nop");
    
    
    self->display = lv_disp_create( kBspConst__SCREEN_WIDTH, kBspConst__SCREEN_HEIGHT );
    lv_disp_set_draw_buffers( self->display, self->gram[0], self->gram[1], kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM*sizeof(lv_color_t), LV_DISP_RENDER_MODE_PARTIAL);


    lv_disp_set_flush_cb( self->display, );

    

    if( self->task_refreash==NULL ){
        xTaskCreate( task_func__refreash, "App Gui - Refreash", kAppConst__GUI_STACK_DEPTH, self, kAppConst__GUI_PRIORITY, self->task_refreash);
        res |= (NULL==self->task_refreash) << 0;
    }
    
    return res;
}


AppGui_t g_AppGui = {
    .launch = launch_function,
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
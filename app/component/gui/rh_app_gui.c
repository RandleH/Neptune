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

/* Private function prototypes -----------------------------------------------*/
static void task_func__refreash( void* param);

#if   (LVGL_VERSION_MAJOR==8) && ((LVGL_VERSION_MINOR==3) || (LVGL_VERSION_MINOR==2))
static void flush_cb( struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
#elif (LVGL_VERSION_MAJOR==9) && (LVGL_VERSION_MINOR==0) && (LVGL_VERSION_PATCH==0)
static void flush_cb( struct _lv_display_t  *disp,     const lv_area_t *area, uint8_t    *px_map);
#else
  #error "Unknown LVGL version."
#endif


/* Private variables ---------------------------------------------------------*/



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



static int launch_function( void){
    /* Initialize LVGL Library */
    lv_init();

    #if   (LVGL_VERSION_MAJOR==8) && ((LVGL_VERSION_MINOR==3) || (LVGL_VERSION_MINOR==2))   // LVGL Version 8.2.X ~ 8.3.X
        lv_disp_drv_init( &self->driver);
        self->driver.draw_buf = &self->buf;
        self->driver.flush_cb = flush_cb;
        self->driver.hor_res = kBspConst__SCREEN_HEIGHT;
        self->driver.ver_res = kBspConst__SCREEN_WIDTH;
        self->driver.direct_mode = false;
        lv_disp_draw_buf_init( &self->buf, self->gram[0], self->gram[1], kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM);
        self->display = lv_disp_drv_register( &self->driver);
    #elif (LVGL_VERSION_MAJOR==9) && (LVGL_VERSION_MINOR==0) && (LVGL_VERSION_PATCH==0)     // LVGL Version 9.0.0
        self->display = lv_disp_create( kBspConst__SCREEN_WIDTH, kBspConst__SCREEN_HEIGHT );
        lv_disp_set_draw_buffers( self->display, self->gram[0], self->gram[1], kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM*sizeof(u16),LV_DISP_RENDER_MODE_PARTIAL);
        lv_disp_set_flush_cb( self->display, flush_cb);
    #else
    #error "Unknown LVGL version."
    #endif
    
    xTaskCreate( task_func__refreash, "GUI Refreash Task", kAppConst__GUI_STACK_DEPTH, NULL, kAppConst__PRIORITY_VERY_IMPORTANT, &self->task_refreash);

    if( self->task_refreash==NULL ){
        return 1;
    }
    
    return OK;
}




#if   (LVGL_VERSION_MAJOR==8) && ((LVGL_VERSION_MINOR==3) || (LVGL_VERSION_MINOR==2))

static void flush_cb(struct _lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *px_map){
    /**
     * @note    Use DMA tranmission to boost refreash rate
    */
    rh_bsp_screen__flush( (BspScreenPixel_t *)px_map, area->x1, area->y1, area->x2, area->y2);

    /* IMPORTANT!!!
     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
    lv_disp_flush_ready(disp);
}


#elif (LVGL_VERSION_MAJOR==9) && (LVGL_VERSION_MINOR==0) && (LVGL_VERSION_PATCH==0)

static void flush_cb(struct _lv_display_t *disp, const lv_area_t *area, uint8_t *px_map){
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to be implemented by you.*/
    // int32_t x, y;
    // for(y = area->y1; y <= area->y2; y++) {
    //     for(x = area->x1; x <= area->x2; x++) {
    //         put_px(x, y, *buf);
    //         buf++;
    //     }
    // }

    /**
     * @note    Use DMA tranmission to boost refreash rate
    */
    rh_bsp_screen__flush( (BspScreenPixel_t *)px_map, area->x1, area->y1, area->x2, area->y2);

    /* IMPORTANT!!!
     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
    lv_disp_flush_ready(disp);
}

#else
  #error "Unknown LVGL version."
#endif



/* Functions -----------------------------------------------------------------*/
static int  request_function( TaskHandle_t from_whom ){


    if( self->screen_owner==NULL || self->screen_owner==from_whom ){
        return true;
    }

    return false;
}

static void  yeild_function( TaskHandle_t from_whom){
    if( self->screen_owner==from_whom ){
        self->screen_owner = NULL;
    }
}



/* Exported Handler ----------------------------------------------------------*/
AppGui_t g_AppGui = {
    .screen_owner = NULL,        /* No one takes ctrl of the screen initially */
    .launch       = launch_function,
    .yeild        = yeild_function,
    .request      = request_function
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
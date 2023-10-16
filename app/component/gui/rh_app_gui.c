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
static void task_func__init( void* param);
static void task_func__demo( void* param);
static void flush_cb(struct _lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);


/* Private variables ---------------------------------------------------------*/
enum AppLaunchTaskList{
    kLaunchTaskList__Refreash = 0,
    kLaunchTaskList__Init = 1
};

static AppTaskUnit_t   launch_list[] = {
    [kLaunchTaskList__Refreash] = {
        .pcName = "GUI Refreash Task",
        .pvParameters = NULL,
        .pvTaskCode = task_func__refreash,
        .usStackDepth = kAppConst__GUI_STACK_DEPTH,
        .uxPriority = kAppConst__PRIORITY_VERY_IMPORTANT
    },
    [kLaunchTaskList__Init] = {
        .pcName = "GUI Init Task",
        .pvParameters = NULL,
        .pvTaskCode = task_func__init,
        .usStackDepth = 512U,
        .uxPriority = kAppConst__PRIORITY_VERY_IMPORTANT+1
    }
};

static AppTaskUnit_t   run_list[] = {
    {
        .pcName = "GUI Demo",
        .pvParameters = NULL,
        .pvTaskCode = task_func__demo,
        .usStackDepth = 512U,
        .uxPriority = kAppConst__PRIORITY_IMPORTANT
    }
};

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

static void task_func__init( void* param){
    /* Initialize LVGL Library */
    lv_init();
    
    self->display = lv_disp_create( kBspConst__SCREEN_WIDTH, kBspConst__SCREEN_HEIGHT );
    lv_disp_set_draw_buffers( self->display, self->gram[0], self->gram[1], kAppConst__GUI_NUM_OF_PIXEL_PER_GRAM*sizeof(u16),LV_DISP_RENDER_MODE_PARTIAL);
    
    lv_disp_set_flush_cb( self->display, flush_cb);
}

static void task_func__demo( void* param){
    lv_obj_t* ui_demoscr = lv_obj_create(NULL);
    lv_obj_clear_flag( ui_demoscr, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_radius(ui_demoscr, kBspConst__SCREEN_HEIGHT/2, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_demoscr, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_demoscr, LV_GRAD_DIR_NONE, LV_PART_MAIN| LV_STATE_DEFAULT);


    lv_obj_t* ui_demotxt = lv_label_create(ui_demoscr);
    lv_obj_set_width( ui_demotxt, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_demotxt, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align( ui_demotxt, LV_ALIGN_CENTER );
    lv_label_set_text(ui_demotxt,"Tap Me");

    lv_obj_set_style_text_opa(ui_demotxt, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_demotxt, &lv_font_montserrat_48, LV_PART_MAIN| LV_STATE_DEFAULT);

    const lv_color_t a[] = { lv_color_hex(0xFF0000), lv_color_hex(0x00FF00), lv_color_hex(0x0000FF) };
    u8 idx = 0;
    while(1){
        lv_obj_set_style_text_color(ui_demotxt, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_color(ui_demoscr, a[idx++], LV_PART_MAIN | LV_STATE_DEFAULT );
        
        idx %= sizeof(a)/sizeof(*a);

        if( self->request( xTaskGetCurrentTaskHandle()) ){
            lv_disp_load_scr( ui_demoscr);
        }

        vTaskDelay(1000);
    }
}

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


/* Functions -----------------------------------------------------------------*/
static int  request_function( TaskHandle_t from_whom ){
    if( self->launch_list[kLaunchTaskList__Init].pxCreatedTask==NULL){
        return 1;
    }


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
    .launch_list = launch_list,
    .launch_list_len = sizeof(launch_list)/sizeof(*launch_list),
    .run_list = run_list,
    .run_list_len = sizeof(run_list)/sizeof(*run_list),
    .yeild = yeild_function,
    .request = request_function
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
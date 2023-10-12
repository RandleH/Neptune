
/**
  ******************************************************************************
  * @file    rh_app_trace.c
  * @author  RandleH
  * @brief   Application: Trace
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
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rh_app_trace.h"
#include "rh_app_task.h"
#include "rh_cmn.h"



/* Private define ------------------------------------------------------------*/
#define self                                (&g_AppTaskMgr)









static void task_main_entrance( void* pTaskUnit){
    
    ((AppTaskUnitInternal_t*)pTaskUnit)->func( ((AppTaskUnitInternal_t*)pTaskUnit)->param );

}



static int lunch_function( void){
    self->tc_list_mask_len = 1;
    self->tc_list          = (AppTaskUnitInternal_t*)pvPortMalloc(((self->tc_list_mask_len)*8*sizeof(AppTaskUnitInternal_t)));
    self->tc_list_mask     = (u8*)pvPortMalloc(self->tc_list_mask_len);
    
    if( !self->tc_list || !self->tc_list_mask ){
        return 1;
    }

    memset( self->tc_list,      0, ((self->tc_list_mask_len)*8*sizeof(AppTaskUnitInternal_t)));
    memset( self->tc_list_mask, 0, self->tc_list_mask_len);
    
    return 0;
}


static int exit_function( int status){



}
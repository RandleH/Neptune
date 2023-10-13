
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



#define IS_BUSY_AT( client_ptr, mask_idx, mask_bit)            (0==(((client_ptr)->tc_list_mask[(mask_idx)]) & (1<<(mask_bit))))

#define TASK_LIST_LENGTH( client_ptr)              ((unsigned)((client_ptr)->tc_list_mask_len<<3))
#define TASK_LIST_LENGTH_IN_BYTES( client_ptr)     ((unsigned)(TASK_LIST_LENGTH((client_ptr))*sizeof(self->tc_list[0])))


static u32 inline util__get_num_of_empty_task( void){
    u32 res = 0;
    for( size_t i=0; i<self->tc_list_mask_len; ++i){
        res += rh_cmn_math__numOf1_8bits( self->tc_list_mask[i] );
    }
    return res;
}

static void task_main_entrance( void* pTaskUnit){

    rh_cmn__assert( pTaskUnit!=NULL, "Invalid Task Entrance Unit");
    
    ((AppTaskUnitInternal_t*)pTaskUnit)->func( ((AppTaskUnitInternal_t*)pTaskUnit)->param );
    
    vTaskDelete( ((AppTaskUnitInternal_t*)pTaskUnit)->handle);
}



static int launch_function( void){
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

static int create( AppTaskUnit_t list[], size_t nItems ){
    u32 num_of_available = util__get_num_of_empty_task();
    u32 num_to_allocate  = num_of_available<nItems ? (((nItems-num_of_available)/8)+ (0!=((nItems-num_of_available)%8)) ) : 0;

    if( num_to_allocate!=0 ){
        //...//
    }

    for( size_t i=0,mask_idx=0,mask_bit=0; i<nItems; ++i){
        
        while( mask_idx<self->tc_list_mask_len ){
            while( IS_BUSY_AT( self, mask_idx, mask_bit) && mask_bit<8){
                ++mask_bit;
            }
            if( mask_bit!=8 ){
                break;
            }
            mask_bit = 0;
            ++mask_idx;
        }
        
        self->tc_list[ (mask_idx*8)+mask_bit ].func  = list[i].pvTaskCode;
        self->tc_list[ (mask_idx*8)+mask_bit ].param = list[i].pvParameters;
        self->tc_list[ (mask_idx*8)+mask_bit ].depth = list[i].usStackDepth;

        xTaskCreate( task_main_entrance, list[i].pcName, list[i].usStackDepth, &self->tc_list[ (mask_idx*8)+mask_bit ], list[i].uxPriority, list[i].pxCreatedTask);

        self->tc_list[ (mask_idx*8)+mask_bit ].handle = *list[i].pxCreatedTask;
    }
}

static int kill_function( TaskHandle_t t){
#ifdef RH_APP_DEBUG

#endif
    u32 mask_idx=0, mask_bit=0;
    while( mask_idx<self->tc_list_mask_len ){
        while( !IS_BUSY_AT( self, mask_idx, mask_bit) && mask_bit<8){
            ++mask_bit;
        }
        if( mask_bit!=8 && self->tc_list[ (mask_idx*8)+mask_bit ].handle==t ){
            break;
        }else{
            mask_bit = 0;
        }

        ++mask_idx;
    }

    rh_cmn__assert( self->tc_list[ (mask_idx*8)+mask_bit ].handle==t, "Impossible");

#if defined (RH_APP_CFG__TASK_MGR_DEBUG) && (RH_APP_CFG__TASK_MGR_DEBUG)==(true)

    g_AppTrace.printf( "Task Killed:                   %s\n",\
        pcTaskGetName(self->tc_list[ (mask_idx*8)+mask_bit ].handle));
    g_AppTrace.printf( "Task Memory History Usage:     %d/%d\t bytes\n",\
        pcTaskGetName(self->tc_list[ (mask_idx*8)+mask_bit ].handle),\ 
        sizeof(StackType_t)*(self->tc_list[ (mask_idx*8)+mask_bit ].depth-uxTaskGetStackHighWaterMark2(self->tc_list[ (mask_idx*8)+mask_bit ].handle)),\
        sizeof(StackType_t)*self->tc_list[ (mask_idx*8)+mask_bit ].depth);
    
#endif

    self->tc_list[ (mask_idx*8)+mask_bit ].handle = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].func   = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].param  = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].depth  = 0;
    
    self->tc_list_mask[mask_idx] &= (u8)(~(1<<mask_bit));
    vTaskDelete(t);
}

static int exit_function( int status){
    
}



AppTask_t g_AppTaskMgr = {
    .launch = launch_function,
    .kill   = kill_function
};



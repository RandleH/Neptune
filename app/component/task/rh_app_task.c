
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
#include "rh_watch.h"
#include "rh_cmn.h"



/* Private define ------------------------------------------------------------*/
#define self                                (&g_AppTaskMgr)
#define IS_BUSY_AT( client_ptr, mask_idx, mask_bit)            (0==(((client_ptr)->tc_list_mask[(mask_idx)]) & (1<<(mask_bit))))
#define TASK_LIST_LENGTH( client_ptr)                          ((unsigned)((client_ptr)->tc_list_mask_len<<3))
#define TASK_LIST_LENGTH_IN_BYTES( client_ptr)                 ((unsigned)(TASK_LIST_LENGTH((client_ptr))*sizeof(self->tc_list[0])))




static u32 util__get_num_of_busy_slots( void);
static u32 util__get_num_of_idle_slots( void);



/**
 * @brief   Utility Function: Get num of available buffer slots to store task information
 * @note    Returned answer should NEVER exceed `self->tc_list_mask_len * 8`. Since that is the maximum bits count that can indicated idle or busy.
 * @return  Return answer
*/
static u32 inline util__get_num_of_busy_slots( void){
    return (self->tc_list_mask_len<<3) - util__get_num_of_idle_slots();
}

static u32 inline util__get_num_of_idle_slots( void){
    u32 res = 0;
    for( size_t i=0; i<self->tc_list_mask_len; ++i){
        res += rh_cmn_math__numOf1_8bits( self->tc_list_mask[i] );
    }
    return res;
}

static u32 inline util__get_buffer_slot_location( TaskHandle_t t, u8 *mask_idx, u8 *mask_bit){
    #warning "Optimized to hash search"
    
    u8 idx = 0;
    u8 bit = 0;

    // for( idx=0; idx<self->tc_list_mask_len; ++idx){
    //     for( bit=0; bit<8; ++bit){    
    //         if( IS_BUSY_AT( self, idx, bit) ){
    //             if(self->tc_list[ idx*8+bit].handle==t){
    //                 *mask_bit = bit;
    //                 *mask_idx = idx;
    //             }
    //         }
    //         ++bit;
    //     }
    // }
    
    
    while( idx<self->tc_list_mask_len){
        bit=0;
        while( bit<8 ){

            u8 mask = self->tc_list_mask[idx];

            u8 res  = (0==(mask & (1<<bit)));

            // if( IS_BUSY_AT( self, idx, bit) ){
            if( res==1 ){
                u8 tmp = (self->tc_list[ idx*8+bit].handle==t);
                if( tmp==1){
                    *mask_bit = bit;
                    *mask_idx = idx;
                    return 0;
                }
            }
            ++bit;
        }
        ++idx;
    }

    return 1;   /* Impossible, check memory leak */
}

static u32 inline util__set_buffer_slot_location( TaskHandle_t t, u8 *mask_idx, u8 *mask_bit){
    #warning "Optimized to hash insert"

    for( *mask_idx=0; *mask_idx<self->tc_list_mask_len; ++*mask_idx){
        for( *mask_bit=0; *mask_bit<8; ++*mask_bit){    
            if( !IS_BUSY_AT( self, *mask_idx, *mask_bit) ){
                return 0;
            }
            ++*mask_bit;
        }
    }

    return 1;   /* Buffer full, please reallocate memory */
}

static void task_main_entrance( void* pTaskUnit);
static int launch_function( void);
static int create_function( AppTaskUnit_t list[], size_t nItems );
static void report_function( void* param);
static int kill_function( TaskHandle_t t, int status);


/**
 * @brief   Internal Function: All task created by `TaskMgr` will call this before entering
 * @param   pTaskUnit   This pointer should cast to `AppTaskUnitInternal_t`
 * @note    `pTaskUnit->func` is the user task code callback. `pTaskUnit->param` is the parameter passed through the callback.
 * @note    If a task created by TaskMgr. Its task code can run without infinite loop. It will result in a task delete.
*/
static void task_main_entrance( void* pTaskUnit){
    rh_cmn__assert( pTaskUnit!=NULL, "Invalid Task Entrance Unit");
    
    ((AppTaskUnitInternal_t*)pTaskUnit)->func( ((AppTaskUnitInternal_t*)pTaskUnit)->param );
    
    self->kill( ((AppTaskUnitInternal_t*)pTaskUnit)->handle, 0);
}


/**
 * @brief   External Function: `TaskMgr` launch function.
 * @note    `launch()` should run before all other external function was called
 * @retval  Return 0 if success   
*/
static int launch_function( void){
    self->tc_list_mask_len = 1;
    self->tc_list          = (AppTaskUnitInternal_t*)pvPortMalloc(((self->tc_list_mask_len)*8*sizeof(AppTaskUnitInternal_t)));
    self->tc_list_mask     = (u8*)pvPortMalloc(self->tc_list_mask_len);
    
    if( !self->tc_list || !self->tc_list_mask ){
        return 1;
    }

    memset( self->tc_list,      0x00, ((self->tc_list_mask_len)*8*sizeof(AppTaskUnitInternal_t)));
    memset( self->tc_list_mask, 0xff, self->tc_list_mask_len);


    xTaskCreate( report_function, "Task Manger Report", 768U, NULL, kAppConst__PRIORITY_DOCUMENTATION, &self->task_report);

    if( self->task_report==NULL){
        return 1;
    }
    
    return OK;
}

/**
 * @brief   External Function: Create a list of tasks.
 * @note    This function may cause memory reallocation inside the `TaskMgr`
 * @note    This function will change the `pxCreatedTask` for each iterm inside the `list[]` which is the task handle. User can decide whether dump or save it.
 * @retval  Return 0 if success.
*/
static int create_function( AppTaskUnit_t list[], size_t nItems ){
    u32 num_of_available = util__get_num_of_idle_slots();
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
        self->tc_list_mask[mask_idx] &= (u8)(~(1<<mask_bit));

        taskENTER_CRITICAL();
        xTaskCreate( task_main_entrance, list[i].pcName, list[i].usStackDepth, &self->tc_list[ (mask_idx*8)+mask_bit ], list[i].uxPriority, &list[i].pxCreatedTask);
        self->tc_list[ (mask_idx*8)+mask_bit ].handle = list[i].pxCreatedTask;
        taskEXIT_CRITICAL();
        
    }
    return 0;
}


/**
 * @brief   External Function: Generate human readable report of all tasks created by `TaskMgr`
 * @note    This function will call `AppTrace` application to help export the message
 * @note    `RH_APP_CFG__TASK_MGR_DEBUG` MUST set to `1`
 * @retval  Return 0 if success.
*/
#if defined (RH_APP_CFG__TASK_MGR_DEBUG) && (RH_APP_CFG__TASK_MGR_DEBUG)==(true)
static void report_function( void* param){
    (void)param;

    HeapStats_t report_heap;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 30000;

    xLastWakeTime = xTaskGetTickCount();

    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency);

        vPortGetHeapStats( &report_heap);
        watch.sys.logger->printf( "============================================================\n");
        watch.sys.logger->printf( "Application Task Manager Report\n");
        watch.sys.logger->printf( "============================================================\n");
        watch.sys.logger->printf( "Heap Memory Usage ------------------------------------------\n");
        watch.sys.logger->printf( " - Number of Free Memory Blocks:              %ld\t\n", report_heap.xNumberOfFreeBlocks);
        watch.sys.logger->printf( " - Minimum Remaining Free Bytes Since Boot:   %ld\tbytes\n", report_heap.xMinimumEverFreeBytesRemaining);
        watch.sys.logger->printf( " - Maximum Allocable Bytes:                   %ld\tbytes\n", report_heap.xSizeOfLargestFreeBlockInBytes);
        watch.sys.logger->printf( " - Total Remaining Free Bytes:                %ld\tbytes\n", report_heap.xAvailableHeapSpaceInBytes);
        watch.sys.logger->printf( " - Total Heap Size:                           %ld\tbytes\n", configTOTAL_HEAP_SIZE);
        watch.sys.logger->printf( " - Number of calls to pvPortMalloc()          %ld\t\n", report_heap.xNumberOfSuccessfulAllocations);
        watch.sys.logger->printf( " - Number of calls to pvPortFree()            %ld\t\n", report_heap.xNumberOfSuccessfulFrees);
        watch.sys.logger->printf( "\n\n");

        watch.sys.logger->printf( "Task Statistic ---------------------------------------------\n");
        watch.sys.logger->printf( " - Total Number of Tasks                      %ld\t\n", uxTaskGetNumberOfTasks());
        
        /**
         * @note        Task List Mask Example
         * @example     | ... | 1 0 1 1 0 0 0 0 | 0 0 1 0 1 0 0 1 |     ; Mask bit preview
         *              | ... |-----Mask[1]-----|-----Mask[0]-----|     ; Mask Array
         *              |---------------Mask Length---------------|     ; Mask Array Length
         * 
         * @note        In this case, mask[1]=(0xB0), mask[0]=(0x29). Each mask bit represents the state of tc_list.
         *                  tc_list_mask[0].bit[0] = 1      ->  tc_list[0]  doesn't contain any task information. Buffer slot is idle
         *                  tc_list_mask[0].bit[1] = 0      ->  tc_list[1]  has valid task info. Buffer slot is busy
         *                  tc_list_mask[0].bit[2] = 0      ->  tc_list[2]  has valid task info. Buffer slot is busy
         *                  ...
         *                  tc_list_mask[x].bit[y] = a      ->  tc_list[ x*8+y ] valid if a==0 else idle(invalid)
        */

        size_t mask_idx = 0;
        u8     mask_bit = 0;

        while( mask_idx<self->tc_list_mask_len ){
            while( mask_bit<8){
                if( IS_BUSY_AT( self, mask_idx, mask_bit)){
                    TaskHandle_t handle = self->tc_list[ (mask_idx*8)+mask_bit ].handle;
                    watch.sys.logger->printf( " -- Task Name:                                %s\n",\
                        pcTaskGetName( handle));
                    watch.sys.logger->printf( " -- Task Stack Peak Usage:                    %ld/%ld\n",\
                        self->tc_list[ (mask_idx*8)+mask_bit ].depth-uxTaskGetStackHighWaterMark2(handle),\
                        self->tc_list[ (mask_idx*8)+mask_bit ].depth);
                }
                ++mask_bit;
            }
            mask_bit = 0;
            ++mask_idx;
        }
        watch.sys.logger->printf( "\n\n""\033[0m");
    }

    

}
#endif


/**
 * @brief   External Function: Given a task handle, kill the task.
 * @param   t   Task handle
 * @retval  Return 0 if success
*/
static int kill_function( TaskHandle_t t, int status){
    if( t==NULL){
        t = xTaskGetCurrentTaskHandle();
    }
    
    u8 mask_idx=0;
    u8 mask_bit=0;

    int res = util__get_buffer_slot_location( t, &mask_idx, &mask_bit);
    rh_cmn__assert( 0==res, "Impossible");

#if defined (RH_APP_CFG__TASK_MGR_DEBUG) && (RH_APP_CFG__TASK_MGR_DEBUG)==(true)

    g_AppTrace.printf( "Task Killed:                   %s\n",\
        pcTaskGetName(self->tc_list[ (mask_idx*8)+mask_bit ].handle));

    g_AppTrace.printf( "Task Returned Status:          %d\n",\
        status);
        
    g_AppTrace.printf( "Task Memory History Usage:     %d/%d\t bytes\n",\
        self->tc_list[ (mask_idx*8)+mask_bit ].depth-uxTaskGetStackHighWaterMark2(self->tc_list[ (mask_idx*8)+mask_bit ].handle),\
        self->tc_list[ (mask_idx*8)+mask_bit ].depth);
    
#endif

    self->tc_list[ (mask_idx*8)+mask_bit ].handle = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].func   = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].param  = NULL;
    self->tc_list[ (mask_idx*8)+mask_bit ].depth  = 0;
    
    self->tc_list_mask[mask_idx] |= (u8)((1<<mask_bit));
    vTaskDelete(t);
    return 0;
}



/* Exported variable ---------------------------------------------------------*/
AppTask_t g_AppTaskMgr = {
    .launch = launch_function,
    .create = create_function,
    .kill   = kill_function
};



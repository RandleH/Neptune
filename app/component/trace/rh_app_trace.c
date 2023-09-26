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
#include "rh_cmn.h"



/* Private define ------------------------------------------------------------*/
#define self                                (&g_AppTrace)
#define BUSY                                (0)
#define IDLE                                (1)

#define RET_OK                              (0)

#define IS_ALL_FULL( client_ptr)            ((u8)(BUSY==(((client_ptr)->activity.mask_tx)&((client_ptr)->activity.mask_rx))))
#define IS_TX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->activity.mask_tx)))
#define IS_RX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->activity.mask_rx)))
#define IS_TX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->activity.mask_tx & (1 << (idx)))))
#define IS_RX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->activity.mask_rx & (1 << (idx)))))
#define SET_TX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->activity.mask_tx |=  (1 << (idx)); }while(0)
#define SET_RX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->activity.mask_rx |=  (1 << (idx)); }while(0)
#define SET_TX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->activity.mask_tx &=  (~(1 << (idx))); }while(0)
#define SET_RX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->activity.mask_rx &=  (~(1 << (idx))); }while(0)
#define GET_IDX( client_ptr, buffer_addr )  ((AppTraceUnit_t*)(buffer_addr) - (AppTraceUnit_t*)(&((client_ptr)->activity.data[0])));

/* Private function prototypes -----------------------------------------------*/
static u8 get_buffer_usage( void);
static void adjust_priority_task_tx( void);
static u32  find_next_available( size_t length, size_t *offset, u32 *idx);

static void task_func__tx( void* ptr);
static void task_func__rx( void* ptr);


static u32 launch( void);
static u32 message( const char *fmt, ...);
static inline u32 isEmptyTX( void);
static inline u32 isEmptyRX( void);
static int main_function( int argc, const char*argv[]);
static int exit_function( void);




/* Private functions ---------------------------------------------------------*/
/**
 * @brief       Utility function - Get buffer usage. Return a value in the scale of [0~32]
 * @note        Typically count how many bits was set to 0 in mask
 *              Assertion may oaccurs when return value larger than 32.
 * @return      Return buffer usage <=32
*/
static u8 get_buffer_usage( void){
    u32 value = self->activity.mask_tx & self->activity.mask_rx;
    value = ( value & 0x55555555 ) + ( (value >>1)  & 0x55555555 );
    value = ( value & 0x33333333 ) + ( (value >>2)  & 0x33333333 );
    value = ( value & 0x0f0f0f0f ) + ( (value >>4)  & 0x0f0f0f0f );
    value = ( value & 0x00ff00ff ) + ( (value >>8)  & 0x00ff00ff );
    value = ( value & 0x0000ffff ) + ( (value >>16) & 0x0000ffff );
    rh_cmn__assert( value<=32, "Algorithm error. \"uint32_t\" value will have 32 bitset at most ");

    /* `value` indicates how many '1' in the mask, where '1' means idle buffer */
    return 32-value;
}

/**
 * @brief       Utility function - Adjust priority for transimission task
 * @note        When buffer usage > 30, `force_to_clear` flag will be set. TX task will block processes until buffer get empty.
 *              When buffer usage > 16, raise priority greater than current process.
 *              Otherwise, reset priority to `kAppConst__TRACE_PRIORITY`
 * @return      (none)
*/
static void adjust_priority_task_tx( void){
    u8 usage = get_buffer_usage();
    if( usage>30 ){
        /* Immediately clear buffer */
        self->force_to_clear = true;
    }else if( usage>16 ){
        self->force_to_clear = false;
        /* Raise priority */
        UBaseType_t priority = RH_MAX( uxTaskPriorityGet(NULL)+1, uxTaskPriorityGet(self->task_tx) );
        vTaskPrioritySet( self->task_tx, RH_MIN( priority, (configMAX_PRIORITIES-1)));
    }else{
        self->force_to_clear = false;
        /* Restore priority */
        vTaskPrioritySet( self->task_tx, kAppConst__TRACE_PRIORITY);
    }
}

/**
 * @brief       Utility function - Find next available buffer
 * @note        Try to fit the buffer in the end of linked list. It not, then traverse each mask bit and find the first bit set to `1` which means idle. The buffer idx will never greater than `32` if the function returns  `RET_OK`.
 * @param       length  [in]        Buffer length per requested
 * @param       offset  [out]       The new buffer address will be: data[idx].addr[offset]
 * @param       idx     [out]       The returned buffer idx
 * @return      Return `RET_OK` if success.
 *              Return 1 if no buffer available
*/
static u32 find_next_available( size_t length, size_t *offset, u32 *idx){
    rh_cmn__assert( offset!=NULL && idx!=NULL, "Can NOT return results since given NULL pointers.");
    
    /* In order to save memory, find from the end node of linked list first to see if possible to append */
    /* If so, no need to assign a new buffer */
    if( self->activity.anchor.pHead!=NULL && (void*)(self->activity.anchor.pEnd)!=(void*)(&self->activity.anchor) && PER_BUFFER_SIZE-self->activity.anchor.pEnd->len > length){
        *offset = self->activity.anchor.pEnd->len;
        *idx    = GET_IDX( self, self->activity.anchor.pEnd);
        return RET_OK;
    }

    if( IS_ALL_FULL(self) ){
        return 1;
    }

    u32 mask_all = (self->activity.mask_rx & self->activity.mask_tx);

    *offset = 0;
    *idx = 0;
    while( BUSY==(mask_all&(1<<(*idx))) ){
        ++(*idx);
    }
    return RET_OK;
}

#if RH_APP_CFG__DEBUG
/**
 * @brief       Task function for app general report
 * @note        
*/
static void task_func_report( void* param){



    while(1){

#if RH_APP_CFG__TRACE__ENABLE_STACK_WATERMARK
    int water_mark = self->stack_water_mark;
#else
    int water_mark = -1;
#endif /* RH_APP_CFG__TRACE__ENABLE_STACK_WATERMARK */

        ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
        self->message( "App Trace Self Report -----------------------------\n"\
                       "Buffer Usage: %d/32\n"\
                       "TX Priority: %d\n"\
                       "RX Priority: %d\n"\
                       "Highest Stack Watermark: %d\n",\
                       get_buffer_usage(),\
                       uxTaskPriorityGet(self->task_tx),\
                       uxTaskPriorityGet(self->task_rx),\
                       water_mark);
        /**
        * @note Since self report was time sensitive, send message as soon as possible.
        */
        self->force_to_clear = true;
    }
    
}
#endif /* RH_APP_CFG__DEBUG */


/**
 * @brief       Task function for TX
 * @note        Default priority set to `kAppConst__TRACE_PRIORITY`
 *              If buffer is empty, then it will enter a block state
 * @return      (none & will never exit)
*/
static void task_func__tx( void* ptr){
    while(1){
        
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
        if( self->force_to_clear==true ){
            taskENTER_CRITICAL();
        }
        
        while( self->activity.anchor.pHead!=NULL ){
            u32 idx = (self->activity.anchor.pHead - &self->activity.data[0]);
            rh_cmn__assert( IS_TX_FULL_AT( self, idx)==true, "Empty buffer exists in linked list.");

            rh_cmn_usart__send_dma( self->activity.anchor.pHead->addr, self->activity.anchor.pHead->len, NULL);
                
            /* Request access to shared resource */
            if( xSemaphoreTake( self->lock_handle, portMAX_DELAY)==pdTRUE){
                if( self->activity.anchor.pHead->pNext==NULL ){
                    /* Reached the end of linked list */
                    self->activity.anchor.pEnd = (AppTraceUnit_t*)&self->activity.anchor;
                }
                /* Remove this node from linked list */
                self->activity.anchor.pHead = self->activity.anchor.pHead->pNext;

                /* Reset this node */
                self->activity.data[idx].len   = 0;
                self->activity.data[idx].pNext = NULL;
                self->activity.data[idx].pPrev = NULL;

                /* Set to empty */
                SET_TX_EMPTY_AT( self, idx);

                /* Free shared resource */
                xSemaphoreGive( self->lock_handle);

#if RH_APP_CFG__TRACE__ENABLE_STACK_WATERMARK
                self->stack_water_mark = RH_MAX( self->stack_water_mark, uxTaskGetStackHighWaterMark( NULL ));
#endif /* RH_APP_CFG__TRACE__ENABLE_STACK_WATERMARK */

            }
        }
        if( self->force_to_clear==true ){
            taskEXIT_CRITICAL();
            #warning "Should only affect `self->message()`, not to suspend all process"
        }
        
        adjust_priority_task_tx();

    }
}

/**
 * @brief       Task function for RX
 * @note        
 *              If buffer is empty, then it will enter a block state
 * @return      (none & will never exit)
*/
static void task_func__rx( void* ptr){
    #warning "Unimpliment"
    while(1){
        // uxTaskGetStackHighWaterMark( NULL );
    }
}




/**
 * @brief       Initialize required memory and launch threads.
 * @return      Return 0 if success
 *              Return value should be interreted bitwisely.
 *              Bit[0] --> Receive Thread launch failed. Null handle return.
 *              Bit[1] --> Transmit Thread launch failed. Null handle return.
 *                       
*/
static u32 launch( void){
    u32 res = 0x00000000;
    static StaticTask_t task_tcb_rx = {0};
    g_AppTrace.task_rx = xTaskCreateStatic(     task_func__rx, 
                                                "App Trace - RX", 
                                                sizeof(g_AppTrace.stack)/sizeof(StackType_t), 
                                                &g_AppTrace, 
                                                kAppConst__TRACE_PRIORITY, 
                                                g_AppTrace.stack, 
                                                &task_tcb_rx);

    res |= (NULL==self->task_rx) << 0;
    
    xTaskCreate( task_func__tx, "App Trace - TX", 512U, &g_AppTrace, kAppConst__TRACE_PRIORITY, &g_AppTrace.task_tx);
    res |= (NULL==self->task_tx) << 1;
    
    self->lock_handle = xSemaphoreCreateMutexStatic( &self->lock_buffer );
    res |= (NULL==self->lock_handle)<<2;

    return res;
}


/**
 * @brief       Append format message to buffer. Message will be sent out when in idle process
 * @note        Thread safe function. 
*/
static u32 message( const char *fmt, ...){
    u32     ret    = RET_OK;        /* Value about to return */
    u32     idx    = 0;    /* Idx for available buffer */
    size_t  offset = 0;

    rh_cmn__assert( (0xFFFFFFFF==(self->activity.mask_tx | self->activity.mask_rx)), "TX RX buffer should NOT be overlapped" );
    
    /* Request access to shared resource */
    if( xSemaphoreTake( self->lock_handle, ((TickType_t)kAppConst__TRACE_MAX_WAIT_TICK)==pdTRUE) ){
        /* Find an available buffer */
        if( RET_OK==find_next_available( 1+strlen(fmt), &offset, &idx) ){
            
            /* Found it, then validate the results */
            rh_cmn__assert( idx<32U, "Impossible, slot index is greater than 32 when buffer is NOT full");

            /**
             * @note The combination of `idx` and `offset` can determine where memory can be written to. Here is the usage example.
             * 
             * @example: new_available_location = self->activity.data[idx].addr + offset
             * 
             * @note For memory saving purpose, `find_next_available() may or may NOT provide a buffer location`that was completely empty (marked as idle). Linked list add on only occurs when new empty buffer was designated. Therefore we need to further check the result from `find_next_available()`
            */
            if( false==IS_TX_FULL_AT( self, idx) ){
                /* An idle buffer was designated by `find_next_available()` */
                SET_TX_FULL_AT( self, idx);
                /* Append to the end */
                self->activity.data[idx].pNext    = NULL;
                if( self->activity.anchor.pHead!=NULL ){
                    /* Link list is NOT empty */
                    rh_cmn__assert( ((void*)self->activity.anchor.pEnd!=(void*)(&self->activity.anchor)), "Anchor off hook. Head & end node don't match.");
                    self->activity.anchor.pEnd->pNext = &self->activity.data[idx];
                    self->activity.data[idx].pPrev    = self->activity.anchor.pEnd;
                }else{
                    /* Link list is empty */
                    rh_cmn__assert( (void*)(self->activity.anchor.pEnd)==(void*)(&self->activity.anchor), "Anchor off hook. Head & end node don't match.");
                    self->activity.anchor.pHead       = &self->activity.data[idx];
                    self->activity.data[idx].pPrev    = (AppTraceUnit_t*)&self->activity.anchor;
                }

                /* Terminal node move to this */
                self->activity.anchor.pEnd = &self->activity.data[idx];
            }
        }else{
            /* No available buffer */
            ret = 1;
        }

        /* Free shared resource */
        xSemaphoreGive( self->lock_handle);
    }else{
        /* Busy */
        ret = 2;
    }
    
    if( ret != RET_OK ){
        return ret;
    }

    /**
     * @note The snprintf() and vsnprintf() functions will write at most size-1 of the characters printed into the output string (the size'th character then gets the terminating ‘\0’); if
     the return value is greater than or equal to the size argument, the string was too short and some of the printed characters were discarded.  The output is always null-
     terminated, unless size is 0.
    */
    va_list ap;
    va_start( ap, fmt);
    const int len_max = PER_BUFFER_SIZE-offset;
    int len = vsnprintf( &(self->activity.data[idx].addr[offset]), len_max, fmt, ap);
    va_end(ap);
    
    /* Warning: Given message is too long & unable to print whole */
    if( len >= len_max ){
        rh_cmn__assert( self->activity.data[idx].addr[PER_BUFFER_SIZE-1]=='\0', "String should be null terminated");

        /* Add ellipsis symbol at the end */
        self->activity.data[idx].addr[PER_BUFFER_SIZE-2] = '.';
        self->activity.data[idx].addr[PER_BUFFER_SIZE-3] = '.';
        self->activity.data[idx].addr[PER_BUFFER_SIZE-4] = '.';
        self->activity.data[idx].len             += len_max;
    }else{
        self->activity.data[idx].len             += len;
    }
    /* Trigger to send message through hardware */
    xTaskNotifyGive( self->task_tx);

    /* Adjust priority when necessary */
    adjust_priority_task_tx();

    return RET_OK;
} 



/**
 * @brief       Application exit function
 * @return      Always return 0
*/
static int exit_function( void){
    vTaskDelete( g_AppTrace.task_tx);
    g_AppTrace.task_tx = NULL;

    return 0;
}

/**
 * @brief       Demo function.
 * @param       argc    Num of arguments
 * @param       argv    Array of arguments in string
 * @return      Always return 0
*/
static int main_function( int argc, const char*argv[]){
    static u32 cnt = 0;
    if( g_AppTrace.task_tx==NULL || g_AppTrace.task_rx==NULL ){
        g_AppTrace.launch();
    }
    
    int a,b;
    a=(rand()&0xff); b=(rand()&0xff);
    g_AppTrace.message( "Message %d: %d+%d=%d\n", cnt++, a,b,a+b);
    a=(rand()&0xff); b=(rand()&0xff);
    g_AppTrace.message( "Message %d: %d-%d=%d\n", cnt++, a,b,a-b);
    a=(rand()&0xff); b=(rand()&0xff);
    g_AppTrace.message( "Message %d: %d*%d=%d\n", cnt++, a,b,a*b);
    a=(rand()&0xff); b=(rand()&0xff);
    g_AppTrace.message( "Message %d: %d/%d=%d\n", cnt++, a,b,b==0? 0xFFFFFFFF: (a/b));

    return 0;
}





/**
 * @brief   
*/
static inline u32 isEmptyTX( void){
    return (self->activity.mask_tx==0x00000000 );
}

static inline u32 isEmptyRX( void){
    return (self->activity.mask_rx==0x00000000 );
}



/* Exported variable ---------------------------------------------------------*/
AppTrace_t g_AppTrace = {
    .task_tx     = NULL,
    .task_rx     = NULL,
    .activity    = {    
        .mask_rx=0xFFFFFFFF, 
        .mask_tx=0xFFFFFFFF, 
        .anchor={
            .pHead=NULL, 
            .pEnd=(AppTraceUnit_t*)(&self->activity.anchor)
        }
    },
    .launch      = launch,
    .message     = message,
    .main        = main_function,
    .isEmptyRX   = isEmptyRX,
    .isEmptyTX   = isEmptyTX,
    .exit        = exit_function
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
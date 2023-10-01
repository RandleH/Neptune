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

#define IS_ALL_FULL( client_ptr)            ((u8)(BUSY==(((client_ptr)->buffer.mask_tx)&((client_ptr)->buffer.mask_rx))))
#define IS_TX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->buffer.mask_tx)))
#define IS_RX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->buffer.mask_rx)))
#define IS_TX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->buffer.mask_tx & (1 << (idx)))))
#define IS_RX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->buffer.mask_rx & (1 << (idx)))))
#define SET_TX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->buffer.mask_tx |=  (1 << (idx)); }while(0)
#define SET_RX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->buffer.mask_rx |=  (1 << (idx)); }while(0)
#define SET_TX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->buffer.mask_tx &=  (~(1 << (idx))); }while(0)
#define SET_RX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->buffer.mask_rx &=  (~(1 << (idx))); }while(0)
#define GET_IDX( client_ptr, buffer_addr )  ((AppTraceUnit_t*)(buffer_addr) - (AppTraceUnit_t*)(&((client_ptr)->buffer.slot[0])));

/* Private function prototypes -----------------------------------------------*/
static u8   util__get_buffer_usage( void);
static void util__adjust_priority_task_tx( void);
static u32  util__get_next_available( size_t required_length);

static void task_func__tx( void* ptr);
static void task_func__rx( void* ptr);


static u32 launch( void);
static u32 message( const char *fmt, ...);
static int main_function( int argc, const char*argv[]);
static int exit_function( int);




/* Private functions ---------------------------------------------------------*/
/**
 * @brief       Utility function - Get buffer usage. Return a value in the scale of [0~32]
 * @note        Typically count how many bits was set to 0 in mask
 *              Assertion may oaccurs when return value larger than 32.
 * @return      Return buffer usage <=32
*/
static u8 util__get_buffer_usage( void){
    u32 value = self->buffer.mask_tx & self->buffer.mask_rx;
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
static void util__adjust_priority_task_tx( void){
    u8 usage = util__get_buffer_usage();
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
 *              Return 1 if literally no buffer available. No matter how to assign, always overflowed.
 *              Return 2 if buffer needs to split in order to fit.
*/
static u32  util__get_next_available( size_t required_length){
    
    if( IS_ALL_FULL(self) ){
        return UINT32_MAX;
    }


    u32 idx = UINT32_MAX;

    /* In order to save memory, find from the end node of linked list first to see if possible to append */
    if( self->buffer.anchor.pHead!=NULL && (void*)(self->buffer.anchor.pEnd)!=(void*)(&self->buffer.anchor) && PER_BUFFER_SIZE!=self->buffer.anchor.pEnd->len){

        /* If linked list is NOT empty and end node is NOT full, that's being said still we have some available memory can be appended to the end node, no need to assign a new buffer slot */

        idx    = GET_IDX( self, self->buffer.anchor.pEnd);

        /* However the length is NOT fit, needs to further split */
        // if( PER_BUFFER_SIZE-self->buffer.anchor.pEnd->len < length ){
        //     ret = 2;
        // }
        
    }else{

        /* Otherwise, new buffer slot needs to be assigned */

        u32 mask_all = (self->buffer.mask_rx & self->buffer.mask_tx);

        
        idx = 0;
        while( BUSY==(mask_all&(1<<idx)) ){
            ++idx;
        }
        
        /* However the length is NOT fit, needs to further split */
        if( idx==32 ){
            idx = UINT32_MAX;
        }
    
    }

    
    return idx;
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
        self->message( "App Trace Self Report -----------------------------\r\n"\
                       "Buffer Usage: %d/32\r\n"\
                       "TX Priority: %d\r\n"\
                       "RX Priority: %d\r\n"\
                       "Highest Stack Watermark: %d\r\n",\
                       util__get_buffer_usage(),\
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
        
        while( self->buffer.anchor.pHead!=NULL ){
            u32 idx = (self->buffer.anchor.pHead - &self->buffer.slot[0]);
            rh_cmn__assert( IS_TX_FULL_AT( self, idx)==true, "Empty buffer exists in linked list.");
                
            /* Request access to shared resource */
            if( xSemaphoreTake( self->lock_handle, portMAX_DELAY)==pdTRUE){
                /* Send data non-blockly */
                rh_cmn_usart__send_dma( self->buffer.anchor.pHead->addr, self->buffer.anchor.pHead->len, NULL);

                if( self->buffer.anchor.pHead->pNext==NULL ){
                    /* Reached the end of linked list */
                    self->buffer.anchor.pEnd = (AppTraceUnit_t*)&self->buffer.anchor;
                }
                /* Remove this node from linked list */
                self->buffer.anchor.pHead = self->buffer.anchor.pHead->pNext;

                /* Reset this node */
                self->buffer.slot[idx].len   = 0;
                self->buffer.slot[idx].pNext = NULL;
                self->buffer.slot[idx].pPrev = NULL;

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
        
        util__adjust_priority_task_tx();

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

static inline void push_back_node( u32 idx){
    rh_cmn__assert( idx<32, "Buffer index can NOT greater than 32" );


    self->buffer.slot[idx].pNext    = NULL;
    if( self->buffer.anchor.pHead!=NULL ){
        /* Link list is NOT empty */
        rh_cmn__assert( ((void*)self->buffer.anchor.pEnd!=(void*)(&self->buffer.anchor)), "Anchor off hook. Head & end node don't match.");
        self->buffer.anchor.pEnd->pNext = &self->buffer.slot[idx];
        self->buffer.slot[idx].pPrev    = self->buffer.anchor.pEnd;
    }else{
        /* Link list is empty */
        rh_cmn__assert( (void*)(self->buffer.anchor.pEnd)==(void*)(&self->buffer.anchor), "Anchor off hook. Head & end node don't match.");
        self->buffer.anchor.pHead       = &self->buffer.slot[idx];
        self->buffer.slot[idx].pPrev    = (AppTraceUnit_t*)&self->buffer.anchor;
    }

    /* Terminal node move to this */
    self->buffer.anchor.pEnd = &self->buffer.slot[idx];
}

static inline void pop_front_node( u32 idx){

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
    if( self->task_tx==NULL ){
        /* App uninitialized */
        return 3;
    }

    u32     ret    = RET_OK;        /* Value about to return */
    u32     idx    = 0;    /* Idx for available buffer */
    va_list args1, args2; va_start( args1, fmt); va_copy( args2, args1);
    u32     len    = 1+vsnprintf( NULL, 0, fmt, args1); /* Required length for formatted string, including the terminal symbol '\0' */
    va_end( args1);

    rh_cmn__assert( (0xFFFFFFFF==(self->buffer.mask_tx | self->buffer.mask_rx)), "TX RX buffer should NOT be overlapped" );
    
    /* Request access to shared resource */
    if( xSemaphoreTake( self->lock_handle, 50) ){
        /* Find an available buffer */

        idx = util__get_next_available(len);

        if( idx==UINT32_MAX ){
            /* No available buffer */
            ret = 1;
        }else{
            rh_cmn__assert( idx<32, "Index can NOT exceed 32.");

            /**
             * @note For memory saving purpose, `util__get_next_available() may or may NOT provide a buffer location`that was completely empty (marked as idle). Linked list add on only occurs when new empty buffer was designated. Therefore we need to further check the result from `util__get_next_available()`
            */
            if( false==IS_TX_FULL_AT( self, idx) ){
                /* An idle buffer was designated by `util__get_next_available()` */
                rh_cmn__assert( self->buffer.slot[idx].len==0, "Idle buffer should have a length fo 0.");
                SET_TX_FULL_AT( self, idx);
                /* Append to the end */
                push_back_node(idx);
            }

            if( self->buffer.slot[idx].len+len>PER_BUFFER_SIZE ){
                /* Save temporarily & Copy piece by piece */
                size_t offset       = self->buffer.slot[idx].len;
                size_t extra_len    = len;
                u8 *   extra_buffer = alloca(extra_len);
                vsnprintf( extra_buffer, extra_len, fmt, args2);

                size_t extra_idx = 0;
                memcpy( &(self->buffer.slot[idx].addr[offset]), &extra_buffer[extra_idx], PER_BUFFER_SIZE-offset );
                extra_idx                    += (PER_BUFFER_SIZE-offset);
                self->buffer.slot[idx].len += (PER_BUFFER_SIZE-offset);
                extra_len                    -= (PER_BUFFER_SIZE-offset);

                idx    = 0;
                while( extra_len!=0 ){
                    u32 mask_all = (self->buffer.mask_rx & self->buffer.mask_tx);
                    while( BUSY==(mask_all&(1<<(idx))) && idx<32 ){
                        ++(idx);
                    }
                    if( idx<32 ){
                        SET_TX_FULL_AT( self, idx);
                        push_back_node(idx);
                        memcpy( &(self->buffer.slot[idx].addr[0]), &extra_buffer[extra_idx], RH_MIN(PER_BUFFER_SIZE, extra_len) );
                        extra_idx                    += RH_MIN(PER_BUFFER_SIZE, extra_len);
                        self->buffer.slot[idx].len += RH_MIN(PER_BUFFER_SIZE, extra_len);
                        extra_len                    -= RH_MIN(PER_BUFFER_SIZE, extra_len);
                    }else{
                        rh_cmn__assert( idx==32, "Expected idx==32");
                        break;
                    }
                }

                if( idx==32 && extra_len!=0 ){
                    #warning "TODO"
                }
            }else{
                /* Write to buffer directly */
                /**
                 * @note The snprintf() and vsnprintf() functions will write at most size-1 of the characters printed into the output string (the size'th character then gets the terminating ‘\0’); if the return value is greater than or equal to the size argument, the string was too short and some of the printed characters were discarded.  The output is always null-terminated, unless size is 0.
                */
                vsnprintf( &(self->buffer.slot[idx].addr[self->buffer.slot[idx].len]), PER_BUFFER_SIZE-self->buffer.slot[idx].len, fmt, args2);
                self->buffer.slot[idx].len += len;
            }
        }

        va_end(args2);

        /* Free shared resource */
        xSemaphoreGive( self->lock_handle);
    }else{
        /* Busy */
        ret = 2;
    }

    if( ret==RET_OK ){
        /* Trigger to send message through hardware */
        xTaskNotifyGive( self->task_tx);

        /* Adjust priority when necessary */
        util__adjust_priority_task_tx();
    }
    
    return ret;
} 



/**
 * @brief       Application exit function
 * @return      Always return 0
*/
static int exit_function( int status){
    vTaskDelete( g_AppTrace.task_tx);
    g_AppTrace.task_tx = NULL;

    return status;
}

/**
 * @brief       Demo function.
 * @param       argc    Num of arguments
 * @param       argv    Array of arguments in string
 * @return      Always return 0
*/
static int main_function( int argc, const char*argv[]){
    
    g_AppTrace.message( "App Trace Self Report -----------------------------\r\r\n"\
                        "Buffer Usage: %d/32\r\n"\
                        "TX Priority: %d\r\n"\
                        "RX Priority: %d\r\n"\
                        "Highest Stack Watermark: %d\r\n",\
                       util__get_buffer_usage(),\
                       uxTaskPriorityGet(self->task_tx),\
                       uxTaskPriorityGet(self->task_rx),\
                       self->stack_water_mark);

    return 0;
}



/* Exported variable ---------------------------------------------------------*/
AppTrace_t g_AppTrace = {
    .task_tx     = NULL,
    .task_rx     = NULL,
    .buffer      = {    
        .mask_rx=0xFFFFFFFF, 
        .mask_tx=0xFFFFFFFF, 
        .anchor={
            .pHead=NULL, 
            .pEnd=(AppTraceUnit_t*)(&self->buffer.anchor)
        }
    },
    .launch      = launch,
    .message     = message,
    .main        = main_function,
    .exit        = exit_function
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
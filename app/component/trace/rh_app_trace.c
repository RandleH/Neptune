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
#define RET_BUSY                            (1)

#define INVALID_SLOT_IDX                    (32U)


#define IS_ALL_FULL( client_ptr)            ((u8)(BUSY==(((client_ptr)->buffer.mask_tx)&((client_ptr)->buffer.mask_rx))))
#define IS_TX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->buffer.mask_tx)))
#define IS_RX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->buffer.mask_rx)))
#define IS_TX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->buffer.mask_tx & (1 << (idx)))))
#define IS_RX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->buffer.mask_rx & (1 << (idx)))))
#define IS_EMPTY_LL( client_ptr)            (0xFFFFFFFF==(((client_ptr)->buffer.mask_tx)&((client_ptr)->buffer.mask_rx)))
#define IS_SLOT_EXSIT( client_ptr)          (((client_ptr)->buffer.anchor.pHead!=NULL) && ((void*)((client_ptr)->buffer.anchor.pEnd)!=(void*)(&(client_ptr)->buffer.anchor)))


#define SET_TX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->buffer.mask_tx |=  (1 << (idx)); }while(0)
#define SET_RX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->buffer.mask_rx |=  (1 << (idx)); }while(0)
#define SET_TX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->buffer.mask_tx &=  (~(1 << (idx))); }while(0)
#define SET_RX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->buffer.mask_rx &=  (~(1 << (idx))); }while(0)
#define GET_IDX( client_ptr, buffer_addr )  ((AppTraceUnit_t*)(buffer_addr) - (AppTraceUnit_t*)(&((client_ptr)->buffer.slot[0])));


/**
 * @brief   Private Enumeration. 
 * @note    Only adopted in `util__get_next_available()` to indicate the result
*/
enum SlotStatus {
    kSlotStatusGood      = 0,       /* Perfect slot. Data can completely fit in */
    kSlotStatusCompact   = 1,       /* Data partially fit in. Another slot memory needed which may not always be available */
    kSlotStatusInvalid   = 2,       /* Invalid slot. Do NOT use */
    kSlotStatusPartial   = 3        /* Data ONLY can be partially sent */
};

/**
 * @brief   Private Structure. 
 * @note    Only returned by `util__get_next_available()` to indicate the result
*/
struct SlotInfo{
    enum SlotStatus status;
    u8              idx;
};
typedef struct SlotInfo SlotInfo_t;


/* Private function prototypes -----------------------------------------------*/
static size_t util__get_buffer_remaining_byte( void);
static u8   util__get_buffer_remaining_slot( void);
static void util__adjust_priority_task_tx( void);
static void util__get_next_available( size_t required_length, SlotInfo_t *return_result);
static void util__push_back_node( u32 idx);
static void util__pop_front_node( u32 idx);

static inline void util__transfer_tx( void);
static inline void util__transfer_rx( void);


static void task_func__tx( void* ptr);
static void task_func__rx( void* ptr);


static u32 launch( void);
static int printf_func( const char *fmt, ...);
static int main_function( int argc, const char*argv[]);
static int exit_function( int);




/* Private functions ---------------------------------------------------------*/

/**
 * 
*/
static size_t util__get_buffer_remaining_byte( void){
    if( IS_EMPTY_LL(self) ){
        return (1<<(kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL));
    }

    /**
     * @note    Equivalent Condition: 
     * @param   a  Remaining memory in the last buffer slot := PER_BUFFER_SIZE-self->activity.anchor.pEnd->len   
     * @param   b  Idle buffer slot counts := 32-get_buffer_usage()
     * @retval  ans := a + PER_BUFFER_SIZE * b
    */
    return (size_t)((PER_BUFFER_SIZE<<5) + PER_BUFFER_SIZE - PER_BUFFER_SIZE*util__get_buffer_remaining_slot() - self->buffer.anchor.pEnd->len);
}

/**
 * @brief       Utility function - Get buffer usage. Return a value in the scale of [0~32]
 * @note        Typically count how many bits was set to 0 in mask
 *              Assertion may oaccurs when return value larger than 32.
 * @return      Return buffer usage <=32
*/
static u8 util__get_buffer_remaining_slot( void){
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
    u8 usage = util__get_buffer_remaining_slot();
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
static void util__get_next_available( size_t required_length, SlotInfo_t *return_result){
    rh_cmn__assert( NULL!=return_result, "Can NOT place results to a null pointer.");

    if( IS_SLOT_EXSIT(self)){
        /* Existing slots in use */
        if( self->buffer.anchor.pEnd->len+required_length <= PER_BUFFER_SIZE ){
            /* Best case. Just append to the end */
            return_result->status = kSlotStatusGood;
            return_result->idx    = GET_IDX( self, self->buffer.anchor.pEnd);
        }else if( required_length <= PER_BUFFER_SIZE && false==IS_ALL_FULL(self) ){
            /* New slot assigned. Enough space to fit in */
            return_result->status = kSlotStatusGood;
            return_result->idx    = 0;
            while( BUSY==(self->buffer.mask_rx&self->buffer.mask_tx&(1<<(return_result->idx))) ){
                ++return_result->idx;
            }
            rh_cmn__assert( return_result->idx<32, "Impossible.");
        }else{
            if( required_length <= util__get_buffer_remaining_byte() ){
                return_result->status = kSlotStatusCompact;

                /**
                 * @note Either condition will result in same solution exentually in `printf_func()`
                 * @note If current slot is full, and returned result designats this full slot index, then it will result a passing length of `0` of copying item in `printf_func()`. Error may or may NOT occur. Therefore here to take a further check to avoid that condition.
                */
                if( self->buffer.anchor.pEnd->len==PER_BUFFER_SIZE ){
                    /* Assign a new slot */
                    return_result->idx    = 0;
                    while( BUSY==(self->buffer.mask_rx&self->buffer.mask_tx&(1<<(return_result->idx))) ){
                        ++return_result->idx;
                    }
                    rh_cmn__assert( return_result->idx<32, "Impossible.");
                }else{
                    /* Append to the end */
                    return_result->idx    = GET_IDX( self, self->buffer.anchor.pEnd);
                }
            }else{
                /* Always overflow. Length is too long. Try partially fits */
                if( self->buffer.anchor.pEnd->len==PER_BUFFER_SIZE){
                    return_result->status = kSlotStatusInvalid;
                    return_result->idx    = INVALID_SLOT_IDX;
                }else{
                    return_result->status = kSlotStatusPartial;
                    return_result->idx    = GET_IDX( self, self->buffer.anchor.pEnd);;
                }
            }
        }
    }else{
        /* Empty buffer. All slots are idle */
        if( required_length <= PER_BUFFER_SIZE){
            /* Best case & easy. Just assign a new slot with index 0 */
            return_result->status = kSlotStatusGood;
            return_result->idx    = 0;
        }else{
            if( required_length <= PER_BUFFER_SIZE*32 ){
                /* New slot assigned. But more slots needed */
                return_result->status = kSlotStatusCompact;
                return_result->idx    = 0;
            }else{
                /* Entire buffer (all slots) assigned, still not able to fit. Message is way too long */
                return_result->status = kSlotStatusPartial;
                return_result->idx    = 0;
            }
        }
    }
}


/**
 * @brief       Transfer function TX. No check
 * @note        Keep sending until empty
*/
static inline void util__transfer_tx( void){
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

            /* Set to empty */
            SET_TX_EMPTY_AT( self, idx);

            /* Free shared resource */
            xSemaphoreGive( self->lock_handle);

        }
    }
}

/**
 * @brief       Transfer function RX. No check
 * @note        Keep receiving until FIFO empty
*/
static inline void util__transfer_rx( void){
    #warning "Unimplimented"
}

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
        
        util__transfer_tx();

        if( self->force_to_clear==true ){
            taskEXIT_CRITICAL();
            #warning "Should only affect `self->printf()`, not to suspend all process"
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

static inline void util__push_back_node( u32 idx){
    rh_cmn__assert( idx<32, "Buffer index can NOT greater than 32" );


    self->buffer.slot[idx].pNext    = NULL;
    if( self->buffer.anchor.pHead!=NULL ){
        /* Link list is NOT empty */
        rh_cmn__assert( ((void*)self->buffer.anchor.pEnd!=(void*)(&self->buffer.anchor)), "Anchor off hook. Head & end node don't match.");
        self->buffer.anchor.pEnd->pNext = &self->buffer.slot[idx];
    }else{
        /* Link list is empty */
        rh_cmn__assert( (void*)(self->buffer.anchor.pEnd)==(void*)(&self->buffer.anchor), "Anchor off hook. Head & end node don't match.");
        self->buffer.anchor.pHead       = &self->buffer.slot[idx];
    }

    /* Terminal node move to this */
    self->buffer.anchor.pEnd = &self->buffer.slot[idx];
}

static inline void util__pop_front_node( u32 idx){

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

    if( self->task_rx==NULL ){
        // xTaskCreate( task_func__rx, "App Trace - RX", (kAppConst__TRACE_STACK_DEPTH/2), &g_AppTrace, kAppConst__TRACE_PRIORITY, &g_AppTrace.task_rx);
        res |= (NULL==self->task_rx) << 0;
    }
    
    if( self->task_tx==NULL ){
        xTaskCreate( task_func__tx, "App Trace - TX", (kAppConst__TRACE_STACK_DEPTH/2), &g_AppTrace, kAppConst__TRACE_PRIORITY, &g_AppTrace.task_tx);
        res |= (NULL==self->task_tx) << 1;
    }
    
    if( self->lock_handle==NULL){
        self->lock_handle = xSemaphoreCreateMutexStatic( &self->lock_buffer );
        res |= (NULL==self->lock_handle)<<2;
    }
    
    return res;
}


/**
 * @brief       Append format message to buffer. Message will be sent out when in idle process
 * @note        Thread safe function. 
 * @retval      Return 0 if success
 *              Return 1 if data partially send due to the buffer overflow
 *              Return 2 if failed
*/
static int printf_func( const char *fmt, ...){
    if( self->task_tx==NULL || fmt==NULL ){
        /* App uninitialized */
        return 2;
    }

    u32     ret    = RET_OK;        /* Value about to return */
    va_list args1, args2; va_start( args1, fmt); va_copy( args2, args1);
    u32     len    = 1+vsnprintf( NULL, 0, fmt, args1); /* Required length for formatted string, including the terminal symbol '\0' */
    int     required_len = len;     /* Returned value subject to ISO standard */
    va_end( args1);

    rh_cmn__assert( (0xFFFFFFFF==(self->buffer.mask_tx | self->buffer.mask_rx)), "TX RX buffer should NOT be overlapped" );
    
    /* Request access to shared resource */
    if( xSemaphoreTake( self->lock_handle, 50) ){
        /* Find an available buffer */
        SlotInfo_t slot = {0};
        util__get_next_available(len, &slot);


        if( slot.status==kSlotStatusGood || slot.status==kSlotStatusCompact ){
            /**
             * @note For memory saving purpose, `util__get_next_available() may or may NOT provide a buffer location`that was completely empty (marked as idle). Linked list add on only occurs when new empty buffer was designated. Therefore we need to further check the result from `util__get_next_available()`
            */
            if( false==IS_TX_FULL_AT( self, slot.idx) ){
                /* An idle buffer was designated by `util__get_next_available()` */
                rh_cmn__assert( self->buffer.slot[slot.idx].len==0, "Idle buffer should have a length fo 0.");
                SET_TX_FULL_AT( self, slot.idx);
                /* Append to the end */
                util__push_back_node(slot.idx);
            }
        }

        switch( slot.status){
            case kSlotStatusGood:{
                /**
                 * @note Write to buffer directly
                 * @note The snprintf() and vsnprintf() functions will write at most size-1 of the characters printed into the output string (the size'th character then gets the terminating ‘\0’); if the return value is greater than or equal to the size argument, the string was too short and some of the printed characters were discarded.  The output is always null-terminated, unless size is 0.
                */
                vsnprintf( &(self->buffer.slot[slot.idx].addr[self->buffer.slot[slot.idx].len]), PER_BUFFER_SIZE-self->buffer.slot[slot.idx].len, fmt, args2);
                self->buffer.slot[slot.idx].len += len;
                break;
            }
            case kSlotStatusPartial:
            case kSlotStatusCompact:{
                /* Save temporarily & Copy piece by piece */
                u8 *   extra_buffer = NULL;
                u8 *   extra_buffer_ptr = NULL;
                u32    extra_len    = 0;
                u8     use_heap     = false;

                /**
                 * @brief       FreeRTOS API: `uxTaskGetStackHighWaterMark()`
                 * @note        The stack used by a task will grow and shrink as the task executes and interrupts are processed.  uxTaskGetStackHighWaterMark() returns the minimum amount of remaining stack space that was available to the task since the task started executing - that is the amount of stack that remained unused when the task stack was at its greatest (deepest) value.  This is what is referred to as the stack 'high water mark'.
                 * @note        The value returned is the high water mark in words
                 * @example     On a 32 bit machine a return value of 1 would indicate that 4 bytes of stack were unused. If the return value is zero then the task has likely overflowed its stack. 
                 * @attention   If the return value is close to zero then the task has come close to overflowing its stack.
                */
                /* Check if stack will be overflowed */
                if( slot.status==kSlotStatusPartial ){
                    /* Required length is too long. Use the remaining buffer in bytes instead */
                    len = util__get_buffer_remaining_byte();
                }
                u32 remaining_stack_in_bytes = uxTaskGetStackHighWaterMark(NULL)<<2;
                if( remaining_stack_in_bytes>=len ){
                    use_heap     = false;
                    extra_buffer = alloca(len);
                    extra_len    = len;
                }else{
                    use_heap     = true;
                    extra_buffer = pvPortMalloc(len);
                    extra_len    = len;
                }

                /* Rare condition that both heap & stack is not long enough to store this string */
                if( NULL==extra_buffer ){
                    ret = 3; /* Failed */
                    break;
                }

                /* Preparation before copy. Clean stash */
                vsnprintf( extra_buffer, extra_len, fmt, args2);
                extra_buffer_ptr = extra_buffer;
                memcpy( &(self->buffer.slot[slot.idx].addr[self->buffer.slot[slot.idx].len]), extra_buffer_ptr, PER_BUFFER_SIZE-self->buffer.slot[slot.idx].len );
                extra_buffer_ptr += PER_BUFFER_SIZE-self->buffer.slot[slot.idx].len;
                extra_len        -= PER_BUFFER_SIZE-self->buffer.slot[slot.idx].len;
                self->buffer.slot[slot.idx].len = PER_BUFFER_SIZE;
                

                /* Begin copy loop */
                slot.idx          = 0;
                while( extra_len!=0 && slot.idx<32 ){
                    u32 mask_all    = (self->buffer.mask_rx & self->buffer.mask_tx);
                    u32 copy_length = RH_MIN(PER_BUFFER_SIZE, extra_len);
                    while( BUSY==(mask_all&(1<<(slot.idx))) && slot.idx<32 ){
                        ++(slot.idx);
                    }
                    SET_TX_FULL_AT( self, slot.idx);
                    util__push_back_node(slot.idx);
                    memcpy( self->buffer.slot[slot.idx].addr, extra_buffer_ptr, copy_length );
                    extra_buffer_ptr                += copy_length;
                    extra_len                       -= copy_length;
                    self->buffer.slot[slot.idx].len += copy_length;
                }

                if( use_heap==true ){
                    vPortFree(extra_buffer);
                }

                if( slot.status==kSlotStatusPartial ){
                    rh_cmn__assert( self->buffer.anchor.pEnd->len==PER_BUFFER_SIZE, "Whole buffer should be full.");
                    rh_cmn__assert( IS_ALL_FULL(self)==true                       , "Whole buffer should be full.");
                }
                break;
            }
            case kSlotStatusInvalid:{
                ret = 3;
                break;
            }
            default:{
                rh_cmn__assert( false, "util() returned an impossible results.");
                break;
            }
        }/* End of data import */


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
    
    // return ret;
    return required_len;
} 



/**
 * @brief       Application exit function
 * @return      Always return 0
*/
static int exit_function( int status){
    taskENTER_CRITICAL();
    self->force_to_clear = true;

    if( NULL!=g_AppTrace.task_tx ){
        if( status==0 ){
            util__transfer_tx();
        }
        vTaskDelete( g_AppTrace.task_tx);
    }

    if( NULL!=g_AppTrace.task_rx ){
        if( status==0 ){
            util__transfer_rx();
        }
        vTaskDelete( g_AppTrace.task_rx);
    }

    vSemaphoreDelete( self->lock_handle);
    
    g_AppTrace.task_tx = NULL;
    g_AppTrace.task_rx = NULL;

    
    
    taskEXIT_CRITICAL();

    return status;
}

/**
 * @brief       Demo function.
 * @param       argc    Num of arguments
 * @param       argv    Array of arguments in string
 * @return      Always return 0
*/
static int main_function( int argc, const char*argv[]){

    return 0;
}


/**
 * @brief   Erases any input or output buffered in the given stream.  For output streams this discards any unwritten output.
*/
static int purge_function( void){
    memset( &self->buffer, '\0', sizeof(self->buffer));
    self->buffer.mask_rx = 0xFFFFFFFF;
    self->buffer.mask_tx = 0xFFFFFFFF;
    self->buffer.anchor.pHead = NULL;
    self->buffer.anchor.pEnd  = (AppTraceUnit_t*)(&self->buffer.anchor);
    self->force_to_clear      = false;

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
    .printf      = printf_func,
    .main        = main_function,
    .purge       = purge_function,
    .exit        = exit_function
};


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
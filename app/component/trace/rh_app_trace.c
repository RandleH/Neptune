

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rh_app_trace.h"
#include "rh_cmn.h"


#define self      (&g_AppTrace)
#define BUSY      (0)
#define IDLE      (1)
#define IS_ALL_FULL( client_ptr)            ((u8)(BUSY==(((client_ptr)->activity.mask_tx)&((client_ptr)->activity.mask_rx))))
#define IS_TX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->activity.mask_tx)))
#define IS_RX_FULL( client_ptr)             ((u8)(BUSY==((client_ptr)->activity.mask_rx)))

#define IS_TX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->activity.mask_tx & (1 << (idx)))))
#define IS_RX_FULL_AT( client_ptr, idx)     ((u8)(BUSY==((client_ptr)->activity.mask_rx & (1 << (idx)))))
#define SET_TX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->activity.mask_tx |=  (1 << (idx)); }while(0)
#define SET_RX_EMPTY_AT( client_ptr, idx)   do{ (client_ptr)->activity.mask_rx |=  (1 << (idx)); }while(0)
#define SET_TX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->activity.mask_tx &=  (~(1 << (idx))); }while(0)
#define SET_RX_FULL_AT( client_ptr, idx)    do{ (client_ptr)->activity.mask_rx &=  (~(1 << (idx))); }while(0)


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


static void task_func__tx( void* ptr){
    static UBaseType_t mark = 0;
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
                /* Set to empty */
                SET_TX_EMPTY_AT( self, idx);

                if( self->activity.anchor.pHead->pNext==NULL ){
                    /* Reached the end of linked list */
                    self->activity.anchor.pEnd = (AppTraceUnit_t*)&self->activity.anchor;
                }
                /* Remove this node from linked list */
                self->activity.anchor.pHead = self->activity.anchor.pHead->pNext;

                /* Free shared resource */
                xSemaphoreGive( self->lock_handle);
                mark = uxTaskGetStackHighWaterMark( NULL );
            }
        }
        if( self->force_to_clear==true ){
            taskEXIT_CRITICAL();
            #warning "Should only affect `self->message()`, not to suspend all process"
        }
        
        adjust_priority_task_tx();

    }
}

static void task_func__rx( void* ptr){
    while(1){
        // uxTaskGetStackHighWaterMark( NULL );
    }
}


static u32 find_next_available( void){
    register u32 idx = 0;
    register u32 mask_all = (self->activity.mask_rx & self->activity.mask_tx);
    while( BUSY==(mask_all&(1<<idx++)) );
    return idx-1;
}




/**
 * @brief       Initialize required memory and launch thread
 * @return      Return 0 if success
 *              Format [Bit 2][Bit 1][Bit 0]
 *                       |      |      |
 *                       |      |      +---------- 1=Receive Thread launch failed
 *                       |      +----------------- 1=Transmit Thread launch failed
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
    
    res |= (pdFALSE==xTaskCreate( task_func__tx, "App Trace - TX", 512U, &g_AppTrace, kAppConst__TRACE_PRIORITY, &g_AppTrace.task_tx)) << 1;
    
    self->lock_handle = xSemaphoreCreateMutexStatic( &self->lock_buffer );
    res |= (NULL==self->lock_handle)<<2;

    return res;
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
 * @brief       Append format message to buffer. Message will be sent out when in idle process
 * @note   Thread safe function. 
*/
static u32 message( const char *fmt, ...){
    u32 res = 0;        /* Value about to return */
    u8  idx = 0;        /* Idx for available buffer */

    rh_cmn__assert( (0xFFFFFFFF==(self->activity.mask_tx | self->activity.mask_rx)), "TX RX buffer should NOT be overlapped" );
    
    adjust_priority_task_tx();
    
    
    /* Request access to shared resource */
    if( xSemaphoreTake( self->lock_handle, ((TickType_t)kAppConst__TRACE_MAX_WAIT_TICK)==pdTRUE) ){
       
        if( IS_ALL_FULL(self) ){
            /* No buffer available */
            res = 1;
        }else{
            /* Find an available buffer & Set to busy */
            idx = find_next_available();
            rh_cmn__assert( idx<32U, "Impossible, slot index is greater than 32 when buffer is NOT full");
            SET_TX_FULL_AT( self, idx);

            /* Append to the end */
            self->activity.data[idx].pNext    = NULL;
            if( self->activity.anchor.pHead!=NULL ){
                /* Link list is NOT empty */
                rh_cmn__assert( self->activity.anchor.pEnd!=(&self->activity.anchor), "Anchor off hook. Head & end node don't match.");
                self->activity.anchor.pEnd->pNext = &self->activity.data[idx];
                self->activity.data[idx].pPrev    = self->activity.anchor.pEnd;
            }else{
                /* Link list is empty */
                rh_cmn__assert( self->activity.anchor.pEnd==(&self->activity.anchor), "Anchor off hook. Head & end node don't match.");
                self->activity.anchor.pHead       = &self->activity.data[idx];
                self->activity.data[idx].pPrev    = (AppTraceUnit_t*)&self->activity.anchor;
            }

            /* Terminal node move to this */
            self->activity.anchor.pEnd = &self->activity.data[idx];
        }

        /* Free shared resource */
        xSemaphoreGive( self->lock_handle);
    }else{
        /* Busy */
        res = 2;
    }
    
    if( res != 0 ){
        return res;
    }

    /**
     * @note The snprintf() and vsnprintf() functions will write at most size-1 of the characters printed into the output string (the size'th character then gets the terminating ‘\0’); if
     the return value is greater than or equal to the size argument, the string was too short and some of the printed characters were discarded.  The output is always null-
     terminated, unless size is 0.
    */
    va_list ap;
    va_start( ap, fmt);
    const int len_max = sizeof(self->activity.data[0].addr);
    int len = vsnprintf( self->activity.data[idx].addr, len_max, fmt, ap);
    va_end(ap);
    
    /* Warning: Given message is too long & unable to print whole */
    if( len >= len_max ){
        /* Add ellipsis symbol at the end */
        self->activity.data[idx].addr[len_max-2] = '.';
        self->activity.data[idx].addr[len_max-3] = '.';
        self->activity.data[idx].addr[len_max-4] = '.';
        self->activity.data[idx].len             = len_max;
    }else{
        self->activity.data[idx].len             = len;
    }
    /* Trigger to send message through hardware */
    xTaskNotifyGive( self->task_tx);

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



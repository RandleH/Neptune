

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "rh_app_trace.h"
#include "rh_cmn.h"


#define IS_READ_LOCK(x)             (((x)&(1<<0))==0)
#define IS_WRITE_LOCK(x)            (((x)&(1<<1))==0)
#define SET_READ_LOCK(x)            do{(x) |= (1<<0);}while(0)
#define SET_WRITE_LOCK(x)           do{(x) |= (1<<1);}while(0)
#define UNSET_READ_LOCK(x)          do{(x) &= (~(1<<0));}while(0)
#define UNSET_WRITE_LOCK(x)         do{(x) &= (~(1<<1));}while(0)
#define MAX_WAIT_CNT                (1000U)
#define MAX_BUFFER_SIZE             (256U)


typedef struct AppTracePackage{
    u32   color;
    const char* buf;
    struct AppTracePackage *pNext;
}AppTracePackage_t;

typedef struct AppTrace{
    AppTracePackage_t *pHead;
    u8                lock;
    u32               raw;
}AppTrace_t;

typedef struct AppTrace   *pAppTrace;


static AppTrace_t s_context = {0};


void rh_app_trace__init( const pAppTraceCfg config){
    s_context.lock  = 0;
    s_context.pHead = NULL;
    s_context.raw   = 0x00000000;
}




/**
 * @brief       Internal trace append function
 * @param       whom    Which component to trace
 * @param       buf     This buffer assumed to have maximum 256 valid bytes in memory and must be NUL terminated.
 * @param       color   Color in hex format
 * @retval      Return 0 if success
 *              Return 1 if no memory to allocated
 *              Return 2 if too busy and maximum retry count reached
*/
static u32 rh_app_trace__append( enum AppTraceItem whom, const char *buf, u32 color){
    u16    wait_cnt         = 0;
    size_t length           = strlen(buf);
    char * allocated_buffer = NULL; 
    const char * str_prefix = NULL;
    size_t       str_prefix_len = 0;

    /* Determine the prefix string */
    switch (whom){
        case kTraceBusClock:
            str_prefix = "BUS CLOCK:"; break;

        case kTraceMemory:
            str_prefix = "MEMORY:"; break;

        case kTraceSPIRegister:
            str_prefix = "SPI:"; break;
        
        default:
            str_prefix = "UNKNOWN:"; break;
    }
    str_prefix_len = strlen(str_prefix);

    /* Allocate message buffer */
    length += str_prefix_len;
    allocated_buffer = rh_cmn_mem__malloc( length+1);
    if( allocated_buffer!=NULL ){
        return 1;
    }

    /* Copy prefix and message body to this buffer */
    strncpy( allocated_buffer               , str_prefix, length);
    strncpy( allocated_buffer+str_prefix_len, buf       , length-str_prefix_len);
    allocated_buffer[length] = '\0';
    
    /* Make a package that contains this message */
    AppTracePackage_t *pItem = (AppTracePackage_t*)rh_cmn_mem__malloc( sizeof(AppTracePackage_t));
    pItem->color = color;
    pItem->buf   = allocated_buffer;
    pItem->pNext = NULL;
    
    /* Append to the end of the queue */
    while( IS_READ_LOCK(s_context.lock) && wait_cnt<MAX_WAIT_CNT){
        ++wait_cnt;
        rh_cmn_delay_ms__escape(10);
    }
    if( wait_cnt==MAX_WAIT_CNT && IS_READ_LOCK(s_context.lock)){
        goto BUSY;
    }else{
        SET_READ_LOCK(s_context.lock);
        wait_cnt = 0;
    }
    
    SET_READ_LOCK(s_context.lock);
    AppTracePackage_t dummy = { .pNext=s_context.pHead};
    AppTracePackage_t *ptr = &dummy;
    while( ptr->pNext!=NULL ){
        ptr = ptr->pNext;
    }
    
    while( IS_WRITE_LOCK(s_context.lock) && wait_cnt<MAX_WAIT_CNT){
        ++wait_cnt;
        rh_cmn_delay_ms__escape(10);
    }
    if( wait_cnt==MAX_WAIT_CNT && IS_WRITE_LOCK(s_context.lock)){
        goto BUSY;
    }else{
        SET_WRITE_LOCK(s_context.lock);
        wait_cnt = 0;
    }
    
    ptr->pNext = pItem;
    UNSET_WRITE_LOCK(s_context.lock);
    UNSET_READ_LOCK(s_context.lock);
    
    /* Success */
    return 0;

    /* Busy */
BUSY:
    rh_cmn_mem__free(allocated_buffer);
    allocated_buffer = NULL;
    return 2;   
}

/**
 * @retval      Return 0 if success
 *              Return 1 if no memory to allocated
 *              Return 2 if too busy and maximum retry count reached
*/
u32 rh_app_trace__append_message( enum AppTraceItem whom, const char *fmt, ...){
    char buffer[MAX_BUFFER_SIZE] = {0};
    va_list ap;
    va_start( ap, fmt);
    int len = vsnprintf( buffer, MAX_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if( len<0 )
        return -1;

    return rh_app_trace__append( whom, buffer, 0xffffff);
}

u32 rh_app_trace__append_debug( enum AppTraceItem whom, char const *fmt, ...){
    char buffer[MAX_BUFFER_SIZE] = {0};
    va_list ap;
    va_start( ap, fmt);
    int len = vsnprintf( buffer, MAX_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if( len<0 )
        return -1;

    return rh_app_trace__append( whom, buffer, 0xffff00);
}

u32 rh_app_trace__append_error( enum AppTraceItem whom, char const *fmt, ...){
    char buffer[MAX_BUFFER_SIZE] = {0};
    va_list ap;
    va_start( ap, fmt);
    int len = vsnprintf( buffer, MAX_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if( len<0 )
        return -1;

    return rh_app_trace__append( whom, buffer, 0xff0000);
}

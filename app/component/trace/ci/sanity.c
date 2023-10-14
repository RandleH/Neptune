/**
  ******************************************************************************
  * @file    ci/sanity.c
  * @author  RandleH
  * @brief   Sanity test - Application Trace
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "rh_app.h"
#include "rh_app_trace.h"
#include "rh_cmn_usart.h"

#if defined (RH_APP_CFG__ENABLE_CI) && ((RH_APP_CFG__ENABLE_CI)==true)

#ifdef __cplusplus
extern "C"{
#endif

/* Private typedef -----------------------------------------------------------*/
typedef struct TestResult{
    bool res;
    const char *test_name;

}TestResult_t;

#if 0
static int log_p( const char* fmt, ...){

}
#else
#define log_p( fmt, ...)  rh_cmn_usart__printf( fmt, ##__VA_ARGS__)
#endif

#if 1
static int log_f( const char* fmt, ...){
    return 0;
}
#else
#define log_f( fmt, ...)  rh_cmn_disk__fprintf( fmt, __VA_ARG__)
#endif

/* Functions -----------------------------------------------------------------*/
/**
 * @brief   Check params after initialization
*/
static TestResult_t* var_0( TestResult_t *p, u32 seed){
    p->test_name = "InitTest";
    p->res       = true;

    g_AppTrace.purge();

    p->res &= (g_AppTrace.buffer.mask_rx==0xFFFFFFFF);
    p->res &= (g_AppTrace.buffer.mask_tx==0xFFFFFFFF);
    p->res &= (g_AppTrace.buffer.anchor.pEnd!=NULL);
    p->res &= ((void*)(&g_AppTrace.buffer.anchor)==(void*)(g_AppTrace.buffer.anchor.pEnd));
    p->res &= (g_AppTrace.buffer.anchor.pHead==NULL);

    g_AppTrace.purge();
    return p;
}

/**
 * @brief   Check params after initialization
 * @return  Return 0 if passed
*/
static TestResult_t* var_1( TestResult_t *p, u32 seed){
    p->test_name = "InitTest";
    p->res       = true;

    g_AppTrace.purge();
    g_AppTrace.launch();
    g_AppTrace.exit(0);

    p->res &= (g_AppTrace.task_rx==NULL);
    p->res &= (g_AppTrace.task_tx==NULL);
    p->res &= (((void*)g_AppTrace.buffer.anchor.pEnd)==((void*)&g_AppTrace.buffer.anchor));
    p->res &= (g_AppTrace.buffer.anchor.pHead==NULL);

    g_AppTrace.purge();
    return p;
}

/**
 * @brief   Linked list append functional
 * @return  Return 0 if passed
*/
static TestResult_t* var_2( TestResult_t *p, u32 seed){
    taskENTER_CRITICAL();

    p->test_name = "FunctionTX";
    p->res       = true;

    g_AppTrace.purge();
    g_AppTrace.launch();

    /* Deploy Environment */
    const size_t PER_BUFFER_LEN    = (1<<(kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL-5));
    const int    NUM_OF_SLOTS      = sizeof(g_AppTrace.buffer.slot)/sizeof(g_AppTrace.buffer.slot[0]);
    

    int             i_slot = seed % NUM_OF_SLOTS;                /* !< Slot Index (Random) */
    AppTraceUnit_t *p_slot = &g_AppTrace.buffer.slot[i_slot];    /* !< Slot Buffer Pointer (Random) */
    int             l_slot = seed % PER_BUFFER_LEN;              /* !< Slot Data Length (Random) */

    
    g_AppTrace.buffer.mask_tx     |= 1<<i_slot;
    g_AppTrace.buffer.anchor.pHead = p_slot;
    g_AppTrace.buffer.anchor.pEnd  = p_slot;
    p_slot->len  = l_slot;
    ((char*)memset( p_slot->addr, '$', l_slot-1 ))[l_slot-1] = '\0';
    

    /* Ref: message.len=32 */
    char word[PER_BUFFER_LEN];
    u32  len       = seed % PER_BUFFER_LEN;
    ((char*)memset( word, '#', len-1))[len-1] = '\0';
    
    
    /* Check Result before run */
    p->res &= (g_AppTrace.buffer.anchor.pHead->pNext==NULL);
    
    /* Run the function */
    g_AppTrace.printf( "%s", word);

    /* Check Result after run */

    /* Second ll node should not be empty */
    p->res &= (NULL!=g_AppTrace.buffer.anchor.pHead->pNext);

    /* End node pointing to the 2rd node */
    p->res &= (g_AppTrace.buffer.anchor.pEnd==g_AppTrace.buffer.anchor.pHead->pNext);

    /* String content should be matched */
    p->res &= (0==strncmp( g_AppTrace.buffer.anchor.pHead->pNext->addr, word, PER_BUFFER_LEN));
    
    /* String length should be matched */
    p->res &= (g_AppTrace.buffer.anchor.pHead->pNext->len==len);
    
    g_AppTrace.purge();
    g_AppTrace.exit(9);
    
    taskEXIT_CRITICAL();
    return p;
}


/**
 * @brief   Application Trace CI sanity test
 * @return  Return 0 if success
 *          Return 1 if failed
*/
int rh_app_trace__ci_sanity( void){

    TestResult_t* (*var[])(TestResult_t*, u32) = {
        var_0,
        var_1,
        var_2
    };

    TestResult_t   s;
    int cnt = 0;
    int numOfTests = sizeof(var)/sizeof(*var);

    log_p( "APP Trace Sanity Test\r\n");
    
    for( int i=0; i<numOfTests; ++i ){
        var[i]( &s, rand());
        cnt += (s.res==true);
        rh_cmn_usart__rprintf( s.res, '.', "\t%d/%d Test\t#%d:", i+1, numOfTests, i, s.test_name);
    }
    
    log_p( "%d tests passed, %d tests failed out of %d\r\n", cnt, numOfTests-cnt, numOfTests);
    return (cnt!=numOfTests);
}

#ifdef __cplusplus
}
#endif  /* End of `extern "C"` */
#endif  /* End of `RH_APP_CFG__ENABLE_CI` */



/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
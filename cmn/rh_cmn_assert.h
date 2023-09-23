

#ifndef RH_CMN_ASSERT
#define RH_CMN_ASSERT

#include <stdio.h>


#include "rh_cmn_usart.h"

/**
 * @brief       Common Assertion
 * @param       expr    Expression expected true or false
 * @param       reason  Assertion failed reason
*/
#define rh_cmn__assert( expr, reason)\
    do{\
        if( (expr) != true ){\
            rh_cmn_usart__printf( "[ERROR] Assertion triggered by %s()\n", __func__ );\
            rh_cmn_usart__printf( "Reason: %s \t %s: line %d\n", (reason), __FILE__, __LINE__);\
            __disable_irq();\
            while(1);\
        }\
    }while(0)


#endif


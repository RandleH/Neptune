


#ifndef RH_CMN_RTC_H 
#define RH_CMN_RTC_H



#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"



typedef struct CmnRtc{
    
    RTC_HandleTypeDef        hw_handle;

}CmnRtc_t;




int rh_cmn_rtc__init( const char* time_stamp);




extern CmnRtc_t g_CmnRtc;


#endif
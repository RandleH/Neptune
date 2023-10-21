


#ifndef RH_CMN_RTC_H 
#define RH_CMN_RTC_H



#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"

#include "rh_common.h"


typedef struct CmnRtc{
    RTC_HandleTypeDef        hw_handle;
    RTC_DateTypeDef          date;
    RTC_TimeTypeDef          time;
    
}CmnRtc_t;




int rh_cmn_rtc__init( const char* time_stamp);
const char* rh_cmn_rtc__report( void);
void rh_cmn_rtc__update( void);
int rh_cmn_rtc__set( u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second);



extern CmnRtc_t g_CmnRtc;


#endif
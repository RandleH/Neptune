




#include <string.h>

#include "rh_cmn.h"



#define self            (&g_CmnRtc)


/**
 * @retval      0: success
 *              1: Invalid time stamp string pointer
 *              2: Hardware initialization failed
*/
int rh_cmn_rtc__init( const char* time_stamp){
    
    
    /**
     * @name    Time Stamp  __TIME_STAMP__
     * @example Thu Oct 19 22:41:50 2023
     * @note    [0:2]   Weekday
     *          [4:6]   Month
     *          [8:9]   Date
     *          [11:12] Hour
     *          [14:15] Minute
     *          [17:18] Second
     *          [20:23] Year
    */

    RTC_DateTypeDef date = {0};
    RTC_TimeTypeDef time = {0};

    if( time_stamp!=NULL && strlen(time_stamp)==24 ){
        switch( time_stamp[4]+time_stamp[5]+time_stamp[6] ){
            case 'J'+'a'+'n': date.Month = 1; break;
            case 'F'+'e'+'b': date.Month = 2; break;
            case 'M'+'a'+'r': date.Month = 3; break;
            case 291: date.Month = 4; break;
            case 295: date.Month = 5; break;
            case 301: date.Month = 6; break;
            case 299: date.Month = 7; break;
            case 285: date.Month = 8; break;
            case 296: date.Month = 9; break;
            case 294: date.Month = 10; break;
            case 307: date.Month = 11; break;
            case 268: date.Month = 12; break;
            default: break;
        }
        date.Date = (time_stamp[8]-'0')*10+(time_stamp[9]-'0');
        date.Year = /*(mmmddyyyy[7]-'0')*1000+(mmmddyyyy[8]-'0')*100+*/(time_stamp[22]-'0')*10+(time_stamp[23]-'0');
        time.Hours       = (time_stamp[11]-'0')*10+(time_stamp[12]-'0');
        time.Minutes     = (time_stamp[14]-'0')*10+(time_stamp[15]-'0');
        time.Seconds     = (time_stamp[17]-'0')*10+(time_stamp[18]-'0');
    }else{
        return 1;
    }
    
    

    self->hw_handle.Instance            = RTC;
    self->hw_handle.Init.HourFormat     = RTC_HOURFORMAT_24;
    self->hw_handle.Init.AsynchPrediv   = 0x7F;
    self->hw_handle.Init.SynchPrediv    = 0xFF;
    self->hw_handle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    self->hw_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    self->hw_handle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_ENABLE();
    if(HAL_OK!=HAL_RTC_Init(&self->hw_handle)){
        return  2;
    }

    if( 0x5050!=HAL_RTCEx_BKUPRead( &self->hw_handle, RTC_BKP_DR0)){
        HAL_RTC_SetTime( &self->hw_handle, &time, RTC_HOURFORMAT_24);
        HAL_RTC_SetDate( &self->hw_handle, &date, RTC_HOURFORMAT_24);
        HAL_RTCEx_BKUPWrite( &self->hw_handle, RTC_BKP_DR0, 0x5050);
    }
    
    return 0;
}





CmnRtc_t g_CmnRtc;
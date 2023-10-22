




#include <string.h>

#include "rh_cmn.h"



#define self            (&g_CmnRtc)


/**
 * @retval      0: success
 *              1: Invalid time stamp string pointer
 *              2: Invalid time data
 *              3: Hardware initialization failed
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

    if( time_stamp!=NULL && strlen(time_stamp)==24 ){
        switch( time_stamp[4]+time_stamp[5]+time_stamp[6] ){
            case 'J'+'a'+'n': self->date.Month = 1; break;
            case 'F'+'e'+'b': self->date.Month = 2; break;
            case 'M'+'a'+'r': self->date.Month = 3; break;
            case 'A'+'p'+'r': self->date.Month = 4; break;
            case 'M'+'a'+'y': self->date.Month = 5; break;
            case 'J'+'u'+'n': self->date.Month = 6; break;
            case 'J'+'u'+'l': self->date.Month = 7; break;
            case 'A'+'u'+'g': self->date.Month = 8; break;
            case 'S'+'e'+'p': self->date.Month = 9; break;
            case 'O'+'c'+'t': self->date.Month = 10; break;
            case 'N'+'o'+'v': self->date.Month = 11; break;
            case 'D'+'e'+'c': self->date.Month = 12; break;
            default: return 2;
        }
        self->date.Date = (time_stamp[8]-'0')*10+(time_stamp[9]-'0');
        self->date.Year = (time_stamp[22]-'0')*10+(time_stamp[23]-'0');
        self->date.WeekDay = rh_cmn_math__zeller( 2000+self->date.Year, self->date.Month, self->date.Date );

        self->time.Hours       = (time_stamp[11]-'0')*10+(time_stamp[12]-'0');
        self->time.Minutes     = (time_stamp[14]-'0')*10+(time_stamp[15]-'0');
        self->time.Seconds     = (time_stamp[17]-'0')*10+(time_stamp[18]-'0');
    }else{
        return 1;
    }

    HAL_PWR_EnableBkUpAccess();

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);  //  HAL_OK;

    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit);

    __HAL_RCC_RTC_ENABLE();
    
    self->hw_handle.Instance            = RTC;
    self->hw_handle.Init.HourFormat     = RTC_HOURFORMAT_24;
    self->hw_handle.Init.AsynchPrediv   = 0x7F;
    self->hw_handle.Init.SynchPrediv    = 0xFF;
    self->hw_handle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    self->hw_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    self->hw_handle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    
    if(HAL_OK!=HAL_RTC_Init(&self->hw_handle)){
        return  3;
    }

    if( ((u16)(RH_HASH&0xFFFF))!=HAL_RTCEx_BKUPRead( &self->hw_handle, RTC_BKP_DR0)){
        HAL_RTC_SetTime( &self->hw_handle, &self->time, RTC_FORMAT_BIN);
        HAL_RTC_SetDate( &self->hw_handle, &self->date, RTC_FORMAT_BIN);
        HAL_RTCEx_BKUPWrite( &self->hw_handle, RTC_BKP_DR0, (RH_HASH&0xFFFF));
    }

    return 0;
}



/**
 * @brief   Return human readable time stamp string
 * @return  char[24], a null terminated string with format subject to `__TIMESTAMP__`
*/
const char* rh_cmn_rtc__report( void){
    // static char tmp[strlen(__TIMESTAMP__)+1] = {0};
    static char tmp[24+1] = {0};
    
    rh_cmn_rtc__update();
    
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
    const char* str_m[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    const char* str_w[] = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };

    rh_cmn__assert( self->date.WeekDay>=1 && self->date.WeekDay<=7, "Invalid weekday range");
    rh_cmn__assert( self->date.Month>=1 && self->date.Month<=12, "Invalid month range");

    snprintf( tmp, sizeof(tmp), "%s %s %2d %02d:%02d:%02d %4d", str_w[self->date.WeekDay-1], str_m[self->date.Month-1], self->date.Date, self->time.Hours, self->time.Minutes, self->time.Seconds, 2000U+self->date.Year);

    return tmp;
}


void inline rh_cmn_rtc__update( void){
    HAL_RTC_GetTime( &self->hw_handle, &self->time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate( &self->hw_handle, &self->date, RTC_FORMAT_BIN);
}

/**
 * @brief   RTC set date & time
 * @param   year    Range 2000~2225. Only 2000~2225 year can be set due to the hw limit
 * @param   month   Range 1~12 -> Jan ~ Dec
 * @param   day     Range 1~31 -> 1st ~ 31th
 * @param   hour    Range 0~23
 * @param   minute  Range 0~59
 * @param   second  Range 0~59
 * @retval  Return 0 if success
 *          Return 1 if time is invalid
 *          Return 2 if date is invalid
 *          Return 3 if set time failed
 *          Return 4 if set date failed
 *          Return 5 if year is out range.
*/
int rh_cmn_rtc__set( u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second){
    if(false==rh_cmn_math__is_valid_time( hour, minute, second)){
        return 1;
    }

    if( false==rh_cmn_math__is_valid_date( year, month, day)){
        return 2;
    }

    if( year<2000 || year>2255 ){
        return 5;
    }

    self->date.Date    = day;
    self->date.Date    = month;
    self->date.Year    = year-2000;
    self->date.WeekDay = rh_cmn_math__zeller( year, month, day);
    self->time.Hours   = hour;
    self->time.Minutes = minute;
    self->time.Seconds = second;

    if( HAL_OK!=HAL_RTC_SetTime( &self->hw_handle, &self->time, RTC_FORMAT_BIN)){
        return 3;
    }
    
    if( HAL_OK!=HAL_RTC_SetDate( &self->hw_handle, &self->date, RTC_FORMAT_BIN)){
        return 4;
    }
    
    return 0;
}



CmnRtc_t g_CmnRtc;
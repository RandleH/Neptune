




#include <stdio.h>
#include "rh_cmn_math.h"


u8 inline rh_cmn_math__numOf1_32bits( u32 value){

#if 1
    value = ( value & 0x55555555 ) + ( (value >>1)  & 0x55555555 );
    value = ( value & 0x33333333 ) + ( (value >>2)  & 0x33333333 );
    value = ( value & 0x0f0f0f0f ) + ( (value >>4)  & 0x0f0f0f0f );
    value = ( value & 0x00ff00ff ) + ( (value >>8)  & 0x00ff00ff );
    value = ( value & 0x0000ffff ) + ( (value >>16) & 0x0000ffff );
    return value;
#else
    return __builtin_popcount(value);
#endif
}

u8 inline rh_cmn_math__numOf0_32bits( u32 value){
    return rh_cmn_math__numOf1_32bits(~value);
}

u8 inline rh_cmn_math__numOf1_8bits( u8 value){
#if 1
    value = ( value & 0x55 ) + ( (value >>1)  & 0x55 );
    value = ( value & 0x33 ) + ( (value >>2)  & 0x33 );
    value = ( value & 0x0f ) + ( (value >>4)  & 0x0f );
    return value;
#else
    return __builtin_popcount(value);
#endif

    
}

u8 inline rh_cmn_math__numOf0_8bits( u8 value){
    return rh_cmn_math__numOf1_8bits(~value);
}


u8 inline rh_cmn_math__is_valid_date( u16 year, u8 month, u8 day){
    if( day==0 || day>=32 || month>=13 || month==0 )
        return false;
    
    if( day==31){
        if( month<=7 ){
            if( ((month&0x01)==0) )
                return false;
        }else{
            if( ((month&0x01)==1) )
                return false;
        }
    }
    
    /* Leap Year Condition */
    if( month==2 && ((year&0x03)==0x00) && day>28 ){
        return false;
    }
    return true;
}

u8 inline rh_cmn_math__is_valid_time( u8 hour, u8 minute, u8 second){
    return hour<60&&minute<60&&second<60;
}

/**
 * @brief   Get the week day given the date
 * @note    Zeller Algorithm
 * @param   year    Unlimited
 * @param   month   Range 1~12 -> Jan ~ Dec
 * @param   day     Range 1~31 -> 1st ~ 31th
 * @return  Return 1~7 the weekday
*/
u8 inline rh_cmn_math__zeller( u16 year, u8 month, u8 day){
    u16 century = 0;
    if( month<=2){
        month    += 12;
        century   = (year-1)/1000;
        year      = (year+100-1)%100;
    }else{
        century   = year/100;
        year     %= 100;
    }
    u8 res = (day + (13*(month+1))/5 + year + (year>>2) + (century>>2) - (century<<1))%7;
    res = (res+7)%7;

    return res<2 ? res+6 : res-1;
}




i16 inline rh_cmn_math__abs_angle_hour( u8 hour, u8 minute, u8 second){
    if( hour>=12){
        return ((hour-12)*3600/12) + (minute*3600/(12*60)) + (second*3600/(12*60*60));
    }else{
        return ((hour   )*3600/12) + (minute*3600/(12*60)) + (second*3600/(12*60*60));
    }
}


i16 inline rh_cmn_math__abs_angle_minute( u8 minute, u8 second){
    return (minute*3600/60) + (second*3600/(60*60));
}

i16 inline rh_cmn_math__abs_angle_second( u8 second){
    return (second*3600/60);
}

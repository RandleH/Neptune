/**
  ******************************************************************************
  * @file    rh_utility.c
  * @author  RandleH
  * @brief   Utility Source Code (No dependency)
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


#include "rh_app_utility.h"


#ifdef __cplusplus
extern "C"{
#endif



u8 rh_util__zellerCalendarWeekday( u8 year, u8 month, u8 date){
    u8 y = 0;         /* Year of the century (year mod 100) */
    u8 c = 0;         /* Zero-based century */
    u8 m = 0;
    u8 d = date;      /* The day of the month */
    
    if( month<=2 ){
        m = month+12;
        y = (year+100-1)%100;
        c = (2000+year-1)/100;
    }else{
        m = month;
        y = year % 100;
        c = (2000+year)/100;
    }
    
    i8 h = (d + (13*(m+1))/5 + y + y/4 + c/4 - 2*c)%7;
    h = (h+7)%7;
    
    return h<2? h+6 : h-1;
}


#ifdef __cplusplus
}
#endif

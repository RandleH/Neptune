/**
  ******************************************************************************
  * @file    main.c
  * @author  RandleH
  * @brief   Main program entrance
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
#include "rh_bsp.h"

#include "rh_cmn_clk.h"

int main( int argc, char const *argv[]){

  	rh_cmn_clk__cpu( _96MHz);


  	rh_util__zellerCalendarWeekday( 70, 1, 1);
    
	pBspKey key = rh_bsp_key__init();
  	pBspLed led = rh_bsp_led__init();
    
	u32 state = rh_bsp_led__state( led, M_BSP_LED__B);
	rh_bsp_led__toggle( led, M_BSP_LED__B, 90);


  	while(1){
    	u32 tmp = rh_bsp_led__state( led, M_BSP_LED__B);
		if( state != tmp ){
			rh_bsp_led__toggle( led, M_BSP_LED__B, 90);
			state = tmp;
		}
  	}

  return 0;
}

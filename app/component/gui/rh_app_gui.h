/**
  ******************************************************************************
  * @file    rh_app_gui.h
  * @author  RandleH
  * @brief   Application for GUI
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


#ifndef RH_APP_GUI_H
#define RH_APP_GUI_H


/* Includes ------------------------------------------------------------------*/
#include "rh_common.h"
#include "FreeRTOS.h"
#include "task.h"



/* Exported tasks ------------------------------------------------------------*/
int app_gui_main( void);
int app_gui_exit( void);



#endif


/************************ (C) COPYRIGHT RandleH *****END OF FILE***************/
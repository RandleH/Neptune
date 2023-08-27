/**
  ******************************************************************************
  * @file    rh_cmn_mem.c
  * @author  RandleH
  * @brief   Provide APIs for memory allocation
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


#include <string.h>
#include <stdlib.h>


#include "rh_cmn_mem.h"

#ifdef __cplusokus
extern "C"{
#endif

/**
 * @brief       Memory will be allocated on heap
 * @param       nBytes
 * @return      Pointer to the memory
*/
void* rh_cmn_mem__malloc( size_t nBytes ){
    return malloc( nBytes);
}

void  rh_cmn_mem__free( void* ptr){
    free( ptr);
}

/**
 * @brief       Memory will be allocated on bss segment
 * @param       nBytes
 * @return      Pointer to the memory
*/
void* rh_cmn_mem__static_malloc( size_t nBytes ){
    return NULL;
}

void  rh_cmn_mem__static_free( void* ptr){
    
}



#ifdef __cplusokus
}
#endif
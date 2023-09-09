/**
  ******************************************************************************
  * @file    rh_bsp_screen.c
  * @author  RandleH
  * @brief   This file contains code template.
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


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rh_bsp_screen.h"
#include "rh_cmn_gpio.h"
#include "rh_cmn_spi.h"
#include "rh_cmn_delay.h"



/* Private define ------------------------------------------------------------*/
#define CMD    0
#define DAT    1

#define PIN_DC(x)\
    do{\
        ((x)==0)? rh_cmn_gpio__unsetBit( GPIOC, (1<<15)) : rh_cmn_gpio__setBit( GPIOC, (1<<15));\
    }while(0)

#define PIN_CS(x)\
    do{\
        ((x)==0)? rh_cmn_gpio__unsetBit( GPIOC, (1<<14)) : rh_cmn_gpio__setBit( GPIOC, (1<<14));\
    }while(0)

#define PIN_RST(x)\
    do{\
        ((x)==0)? rh_cmn_gpio__unsetBit( GPIOB, (1<<2)) : rh_cmn_gpio__setBit( GPIOB, (1<<2));\
    }while(0)


#define DELAY_MS(x)\
    do{\
        rh_cmn_delay_ms__halt(x);\
    }while(0)


/* Private functions ---------------------------------------------------------*/
/**
 * @brief       
*/
static void rh_bsp_screen__send( const u8 *code, size_t len){
    while( len!=0 ){
        PIN_DC( *code++);
        --len;
        u8 nItem = *code++;
        --len;

        u8 done = false;
        rh_cmn_spi__send_block( code, nItem, &done);
        while(!done){
            //...//
        }
        code += nItem;
        len  -= nItem;
    }
}

static void rh_bsp_screen__reset( void){
    PIN_RST(1);
    DELAY_MS(100);
    PIN_RST(0);
    DELAY_MS(100);
    PIN_RST(1);
    DELAY_MS(100);
}


/* Functions -----------------------------------------------------------------*/
/**
 * @brief       Initialize screen         \n
 *              Estimated Duration: 500ms \n
 * 
 * @return
 * 
*/
u32 rh_bsp_screen__init( void){
    rh_bsp_screen__reset();

    PIN_CS(0);
    DELAY_MS(100);
    
    const u8 init_code[] = {
        CMD,  1, 0x36,\
        DAT,  1, 0xc8,\
        CMD,  2, 0xef,0xeb,\
        DAT,  1, 0x14,\
        CMD,  3, 0xfe,0xef,0xeb,\
        DAT,  1, 0x14,\
        CMD,  1, 0x84,\
        DAT,  1, 0x40,\
        CMD,  1, 0x85,\
        DAT,  1, 0xff,\
        CMD,  1, 0x86,\
        DAT,  1, 0xff,\
        CMD,  1, 0x87,\
        DAT,  1, 0xff,\
        CMD,  1, 0x88,\
        DAT,  1, 0x0a,\
        CMD,  1, 0x89,\
        DAT,  1, 0x21,\
        CMD,  1, 0x8a,\
        DAT,  1, 0x00,\
        CMD,  1, 0x8b,\
        DAT,  1, 0x80,\
        CMD,  1, 0x8c,\
        DAT,  1, 0x01,\
        CMD,  1, 0x8d,\
        DAT,  1, 0x01,\
        CMD,  1, 0x8e,\
        DAT,  1, 0xff,\
        CMD,  1, 0x8f,\
        DAT,  1, 0xff,\
        CMD,  1, 0xb6,\
        DAT,  2, 0x00,0x20,\
        CMD,  1, 0x36,\
        DAT,  1, 0xc8,\
        CMD,  1, 0x3a,\
        DAT,  1, 0x05,\
        CMD,  1, 0x90,\
        DAT,  4, 0x08,0x08,0x08,0x08,\
        CMD,  1, 0xbd,\
        DAT,  1, 0x06,\
        CMD,  1, 0xbc,\
        DAT,  1, 0x00,\
        CMD,  1, 0xff,\
        DAT,  3, 0x60,0x01,0x04,\
        CMD,  1, 0xc3,\
        DAT,  1, 0x13,\
        CMD,  1, 0xc4,\
        DAT,  1, 0x13,\
        CMD,  1, 0xc9,\
        DAT,  1, 0x22,\
        CMD,  1, 0xbe,\
        DAT,  1, 0x11,\
        CMD,  1, 0xe1,\
        DAT,  2, 0x10,0x0e,\
        CMD,  1, 0xdf,\
        DAT,  3, 0x21,0x0c,0x02,\
        CMD,  1, 0xf0,\
        DAT,  6, 0x45,0x09,0x08,0x08,0x26,0x2a,\
        CMD,  1, 0xf1,\
        DAT,  6, 0x43,0x70,0x72,0x36,0x37,0x6f,\
        CMD,  1, 0xf2,\
        DAT,  6, 0x45,0x09,0x08,0x08,0x26,0x2a,\
        CMD,  1, 0xf3,\
        DAT,  6, 0x43,0x70,0x72,0x36,0x37,0x6f,\
        CMD,  1, 0xed,\
        DAT,  2, 0x1b,0x0b,\
        CMD,  1, 0xae,\
        DAT,  1, 0x77,\
        CMD,  1, 0xcd,\
        DAT,  1, 0x63,\
        CMD,  1, 0x70,\
        DAT,  9, 0x07,0x07,0x04,0x0e,0x0f,0x09,0x07,0x08,0x03,\
        CMD,  1, 0xe8,\
        DAT,  1, 0x34,\
        CMD,  1, 0x62,\
        DAT, 12, 0x18,0x0d,0x71,0xed,0x70,0x70,0x18,0x0f,0x71,0xef,0x70,0x70,\
        CMD,  1, 0x63,\
        DAT, 12, 0x18,0x11,0x71,0xf1,0x70,0x70,0x18,0x13,0x71,0xf3,0x70,0x70,\
        CMD,  1, 0x64,\
        DAT,  7, 0x28,0x29,0xf1,0x01,0xf1,0x00,0x07,\
        CMD,  1, 0x66,\
        DAT, 10, 0x3c,0x00,0xcd,0x67,0x45,0x45,0x10,0x00,0x00,0x00,\
        CMD,  1, 0x67,\
        DAT, 10, 0x00,0x3c,0x00,0x00,0x00,0x01,0x54,0x10,0x32,0x98,\
        CMD,  1, 0x74,\
        DAT,  7, 0x10,0x85,0x80,0x00,0x00,0x4e,0x00,\
        CMD,  1, 0x98,\
        DAT,  2, 0x3e,0x07,\
        CMD,  3, 0x35,0x21,0x11
    };

    rh_bsp_screen__send( init_code, sizeof(init_code)/sizeof(u8));

    DELAY_MS(120);
    const u8 on_code[] = {
        CMD, 1, 0x29,\
    };
    rh_bsp_screen__send( on_code, sizeof(on_code)/sizeof(u8));

    DELAY_MS(20);
    PIN_CS(1);

    return 0;
}


u32 rh_bsp_screen__flush( const u8 *buf, u16 xs, u16 ys, u16 xe, u16 ye){
    
}








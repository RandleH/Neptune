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
#include <stdlib.h>         /* Description - C Standard Library */
#include <string.h>         /* Description - C Standard Library */

#include "stm32f4xx.h"      /* Description - STM32 Library */

#include "rh_bsp.h"         /* Path - ${PRJ_DIR}/bsp/rh_bsp.h */
#include "rh_cmn_gpio.h"    /* Path - ${PRJ_DIR}/cmn/rh_cmn_gpio.h */
#include "rh_cmn_spi.h"     /* Path - ${PRJ_DIR}/cmn/rh_cmn_spi.h */
#include "rh_cmn_delay.h"   /* Path - ${PRJ_DIR}/cmn/rh_cmn_delay.h */




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



enum BspScreenTransferMode{
    kBspScreenTransferMode__BLK = 0,
    kBspScreenTransferMode__DMA = 1,
};

typedef struct BspScreen{
    enum BspScreenTransferMode tx_mode;
    BspScreenPixel_t           gram[kBspConst__SCREEN_GRAM_SIZE];
}BspScreen_t;

static struct BspScreen s_context = {0};



/* Private functions ---------------------------------------------------------*/
/**
 * @brief       Internal function. Given a predefined transmission code and transfer it in blocking mode. DMA bypass.
 * @param       code    [in]    Code buffer
 * @param       len     [in]    Buffer length
 * @return      (none)
*/
static inline void rh_bsp_screen__parse( const u8 *code, size_t len){
    while( len!=0 ){
        PIN_DC( *code++);       /* Determine command or data */
        --len;
        u8 nItem = *code++;     /* Parse num of items */
        --len;

        u8 done = false;
        rh_cmn_spi__send_block( code, nItem, 1, 0, &done);
        while(!done){
            //...//
        }
        code += nItem;
        len  -= nItem;
    }
}



/**
 * @brief       Internal function. Hardware reset screen. Cycle waste: 300ms
 * @return      (none)
*/
static void rh_bsp_screen__reset( void){
    PIN_RST(1);
    DELAY_MS(100);
    PIN_RST(0);
    DELAY_MS(100);
    PIN_RST(1);
    DELAY_MS(100);
}

/**
 * @brief       Internal function. Select screen area. DMA bypass.
 * @param       x0  Start X
 * @param       y0  Start Y
 * @param       x1  End X
 * @param       y1  End Y
 * @return      (none)
*/
static void rh_bsp_screen__area( u8 x0, u8 y0, u8 x1, u8 y1){
    const u8 code[] = {
        /* Set the X coordinates */
        CMD, 1, 0x2A,\
        DAT, 4, 0x00,  x0,0x00,  x1,\
        
        /* Set the Y coordinates */
        CMD, 1, 0x2B,\
        DAT, 4, 0x00,  y0,0x00,  y1,\

        CMD, 1, 0x2C
    };
    rh_bsp_screen__parse( code, sizeof(code)/sizeof(*code));
}


/**
 * @brief       Change scan direction. DMA bypass.
 * @param       mode    @ref 0: L->R; U->D;
 *                      @ref 1: R->L; D->U;
 * @return      (none)
*/
static void rh_bsp_screen__scan_mode( u8 mode){
    u8 code[] = {
        CMD, 1, 0x36,\
        DAT, 1, 0x08
    };

    switch(mode){
        case 0: // PASSED
            code[5] = 0x08;
            break;
        case 1: // PASSED
            code[5] = 0xC8;
            break;
        default:
            break;    
    }

    rh_bsp_screen__parse( code, sizeof(code)/sizeof(*code));  
}



/**
 * @brief       Fill screen with single color
 *              Length of the buffer must be valid great or equal than (xe-xs+1)*(ye-ys+1)
 * @return      Return 0, if success
 *              Return 1, if area params are wrong
*/
static u32 rh_bsp_screen__fill( const BspScreenPixel_t color, u16 xs, u16 ys, u16 xe, u16 ye){
    if( xs>xe || ys>ye ){
        return 1;
    }

    u8 *buf = alloca(sizeof(BspScreenPixel_t));

    switch( sizeof(color)){
        case 1:
            buf[0] = (u8)(color&0xff);
            break;
        case 2:
            buf[1] = (u8)(color&0xff);
            buf[0] = (u8)(color>>8);
            break;
        case 4:
            buf[3] = (u8)(color&0xff);
            buf[2] = (u8)((color>>8)&0xff);
            buf[1] = (u8)((color>>16)&0xff);
            buf[0] = (u8)((color>>24)&0xff);
            break;
        default:
            return 2;
    }

    if( s_context.tx_mode==kBspScreenTransferMode__BLK ){
        u8 done = false;
        rh_bsp_screen__area( xs, ys, xe, ye);
        PIN_DC(1);
        rh_cmn_spi__send_block( buf, sizeof(color), (xe-xs+1)*(ye-ys+1), 0, &done);
        if(!done){
            #warning "Note: Do something"
        }
        PIN_CS(1);
    }else if( s_context.tx_mode==kBspScreenTransferMode__DMA ){
        for( int i=0; i<kBspConst__SCREEN_GRAM_SIZE; i+=2){
            s_context.gram[i] = color;
        }

        rh_bsp_screen__area( xs, ys, xe, ye);
        PIN_DC(1);
        u32 total_pix = (xe-xs+1)*(ye-ys+1);
        u32 nItems = (total_pix > kBspConst__SCREEN_GRAM_SIZE)? kBspConst__SCREEN_GRAM_SIZE : total_pix;
        u32 nTimes = (total_pix > kBspConst__SCREEN_GRAM_SIZE)? total_pix/kBspConst__SCREEN_GRAM_SIZE : 1;
        rh_cmn_spi__send_dma( (u8*)s_context.gram, nItems*sizeof(BspScreenPixel_t), nTimes, 0, NULL);
        rh_cmn_spi__send_dma( (u8*)s_context.gram, (total_pix % kBspConst__SCREEN_GRAM_SIZE)*sizeof(BspScreenPixel_t), 1, 0, NULL);
        PIN_CS(1);
    }

    return 0;
}






/* Functions -----------------------------------------------------------------*/
/**
 * @brief       Initialize screen. Estimated Duration: 500ms. Reset -> Initialize -> DMA mode
 * @return      Always return 0
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

    rh_bsp_screen__parse( init_code, sizeof(init_code)/sizeof(u8));

    DELAY_MS(120);
    const u8 on_code[] = {
        CMD, 1, 0x29,\
    };
    rh_bsp_screen__parse( on_code, sizeof(on_code)/sizeof(u8));

    DELAY_MS(20);

    rh_bsp_screen__scan_mode(0);
    
    union{
        struct{
            u16 B : 5;
            u16 G : 6;
            u16 R : 5;
        }channel;
        u16 full;
    }color;

    color.channel.R = 31; // [0:31]
    color.channel.G = 63; // [0:63]
    color.channel.B = 31; // [0:31]

    rh_bsp_screen__set_blk_transfer();
    rh_bsp_screen__fill( color.full, 0, 0, CFG_SCREEN_WIDTH-1, CFG_SCREEN_HEIGHT-1);

    rh_bsp_screen__set_dma_transfer();
    
    PIN_CS(1);
    return 0;
}


/**
 * @brief       Set screen transfer mode to DMA. Transfer mode only affect on data transmission not control command.
 *              Set larger gram buffer will give a significant boost
 * @return      Always return 0
*/
u32 rh_bsp_screen__set_dma_transfer( void){
    s_context.tx_mode = kBspScreenTransferMode__DMA;
    return 0;
}

/**
 * @brief       Set screen transfer mode to blocking. Transfer mode only affect on data transmission not control command.
 * @return      Always return 0
*/
u32 rh_bsp_screen__set_blk_transfer( void){
    s_context.tx_mode = kBspScreenTransferMode__BLK;
    return 0;
}



/**
 * @brief       Flush screen from a given buffer
 *              Length of the buffer must be valid great or equal than (xe-xs+1)*(ye-ys+1)
 * @return      Return 0, if success
 *              Return 1, if area params are wrong
 *              Return 2, if buffer pointer is NULL
*/
u32 rh_bsp_screen__flush( const BspScreenPixel_t *buf, u16 xs, u16 ys, u16 xe, u16 ye){
    if( xs>xe || ys>ye ){
        return 1;
    }

    if( buf==NULL){
        return 2;
    }

    PIN_CS(0);
    rh_bsp_screen__area( xs, ys, xe, ye);

    PIN_DC(1);
    if( s_context.tx_mode==kBspScreenTransferMode__DMA ){
        rh_cmn_spi__send_dma( (u8*)buf, (xe-xs+1)*(ye-ys+1)*sizeof(BspScreenPixel_t), 1, 0, NULL);
    }else if( s_context.tx_mode==kBspScreenTransferMode__BLK){
        u8 done = false;
        rh_cmn_spi__send_block( (const u8*)buf, (xe-xs+1)*(ye-ys+1)*sizeof(BspScreenPixel_t), 1, 0, &done);
        if( !done){
            #warning "Do something"
        }
    }

    PIN_CS(1);

    return 0;
}
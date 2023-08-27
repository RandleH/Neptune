#ifndef RH_BSP_LED_H
#define RH_BSP_LED_H


#include "rh_common.h"




#define M_BSP_LED__R            (1<<0)
#define M_BSP_LED__G            (1<<1)
#define M_BSP_LED__B            (1<<2)
#define M_BSP_LED__ALL          ((M_BSP_LED__R)|(M_BSP_LED__G)|(M_BSP_LED__B))


typedef struct BspLed* pBspLed;

pBspLed rh_bsp_led__init  ( void);
u32     rh_bsp_led__switch( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay, u8 cmd);
u32     rh_bsp_led__toggle( pBspLed pContext, u32 M_BSP_LED__xxxx, u32 delay);

u32     rh_bsp_led__state ( pBspLed pContext, u32 M_BSP_LED__xxxx);


u32     rh_bsp_led__deinit( pBspLed pContext);
#endif
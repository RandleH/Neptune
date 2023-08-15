#ifndef RH_BSP_KEY_H
#define RH_BSP_KEY_H


#include "rh_common.h"



#define M_BSP_KEY__U              ((u32)(1<<0))
#define M_BSP_KEY__D              ((u32)(1<<1))
#define M_BSP_KEY__L              ((u32)(1<<2))
#define M_BSP_KEY__R              ((u32)(1<<3))
#define M_BSP_KEY__A              ((u32)(1<<4))
#define M_BSP_KEY__B              ((u32)(1<<5))
#define M_BSP_KEY__C              ((u32)(1<<6))




#ifdef __cplusplus
extern "C"{
#endif


typedef struct BspKeyCfg_t{
  
  u32 raw;
}*pBspKeyCfg_t;

typedef struct BspKey_t* pBspKey_t;

pBspKey_t   rh_bsp_key__init( void);
u32         rh_bsp_key__read        ( pBspKey_t context, u32 M_BSP_KEY__xxxx);
u32         rh_bsp_key__trigger     ( pBspKey_t context, u32 M_BSP_KEY__xxxx, void (*func)( void*), void* param);
u32         rh_bsp_key__activate    ( pBspKey_t context, u32 M_BSP_KEY__xxxx);
u32         rh_bsp_key__deactivate  ( pBspKey_t context, u32 M_BSP_KEY__xxxx);
void        rh_bsp_key__deinit      ( pBspKey_t context);


#ifdef __cplusplus
}
#endif


#endif
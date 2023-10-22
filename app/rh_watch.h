

#ifndef RH_WATCH_H
#define RH_WATCH_H


#include "rh_app.h"
#include "rh_cmn.h"
#include "rh_bsp.h"

typedef struct WatchTopStructure{

    struct AppList{
        AppClock_t      *clock;

    }app;

    struct SysList{
        AppTrace_t      *logger;            /*!< System Application: Logger Application */
        AppTask_t       *taskmgr;           /*!< System Application: Task Manager Application */
        AppGui_t        *gui;               /*!< User Application: Graph User Interface */
    }sys;

    struct HwList{
        CmnSpi_t        *spi;
        CmnRtc_t        *rtc;
        CmnUsart_t      *usart;
        BspScreen_t     *screen;
    }hw;
    
    void (*entrance)( void*);

}WatchTopStructure_t;


extern const WatchTopStructure_t watch;


#endif
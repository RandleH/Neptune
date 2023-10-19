

#ifndef RH_WATCH_H
#define RH_WATCH_H


#include "rh_app.h"


typedef struct WatchTopStructure{

    struct AppList{
        AppClock_t      *clock;

    }app;

    struct SysList{
        AppTrace_t      *logger;            /*!< System Application: Logger Application */
        AppTask_t       *taskmgr;           /*!< System Application: Task Manager Application */
        AppGui_t        *gui;               /*!< User Application: Graph User Interface */
    }sys;

    
    void (*entrance)( void*);

}WatchTopStructure_t;


extern WatchTopStructure_t watch;




#endif
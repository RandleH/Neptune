

#ifndef RH_WATCH_H
#define RH_WATCH_H


#include "rh_app.h"


typedef struct WatchTopStructure{

    struct AppList{
        AppTrace_t      *logger;            /*!< System Logger Application */
        AppTask_t       *taskmgr;           /*!< Task Manager Application */
    }app;

    
    void (*entrance)( void*);

}WatchTopStructure_t;


extern WatchTopStructure_t watch;




#endif
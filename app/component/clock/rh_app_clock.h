#ifndef RH_APP_CLOCK_H
#define RH_APP_CLOCK_H




#include "lvgl.h"


enum AppClockActiveWidget{
    kAppClockActiveWidget__date     = 0,
    kAppClockActiveWidget__week,
    kAppClockActiveWidget__sunmoon,
    kAppClockActiveWidget__battery,
    kAppClockActiveWidget__needle_min,
    kAppClockActiveWidget__needle_hr,
    kAppClockActiveWidget__needle_sec,
    kNumOfAppClockActiveWidget
};


enum AppClockTheme{
    kAppClockTheme__USC,
    kNumOfAppClockTheme
};



typedef struct AppClock{
    AppDisplyable_t     *isDisplayable;
    
    struct{
        enum AppClockTheme theme;
        lv_obj_t    **active_widget_list;
    }cache;
    
    
}AppClock_t;


extern AppClock_t g_AppClock;

#endif
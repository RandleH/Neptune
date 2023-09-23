#ifndef RH_APP_H
#define RH_APP_H





enum AppConst{
    kAppConst__TRACE_STATIC_STACK_SIZE             = (4096),
    kAppConst__TRACE_MESSAGE_BUFFER_SIZE_POW_LEVEL = (12),     // 2^12 = 4096
    kAppConst__TRACE_PRIORITY                      = (20),
    kAppConst__TRACE_MAX_WAIT_TICK                 = (10)
};



#include "rh_app_trace.h"
#include "rh_app_utility.h"
#include "rh_app_time.h"
#include "rh_app_gui.h"


#endif





#include "rh_common.h"


typedef struct AppTraceCfg{
    u32  max_message_cnt;
    u32  log_level;
}*pAppTraceCfg;





enum AppTraceItem{
    kTraceBusClock,
    kTraceSPIRegister,
    kTraceMemory,
};


void rh_app_trace__init( const pAppTraceCfg config);
u32 rh_app_trace__append_message( enum AppTraceItem whom, const char *fmt, ...);
u32 rh_app_trace__append_debug( enum AppTraceItem whom, char const *fmt, ...);
u32 rh_app_trace__append_error( enum AppTraceItem whom, char const *fmt, ...);


u32 rh_app_trace__report( enum AppTraceItem);
u32 rh_app_trace__dump_data( void);


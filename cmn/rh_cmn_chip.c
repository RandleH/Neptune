

#include <string.h>
#include <stdlib.h>
#include "rh_cmn_chip.h"


CmnChipUID_t* rh_cmn_chip__uid( CmnChipUID_t *result){
    return (CmnChipUID_t*)memcpy( result, (void*)UID_BASE, 12U);
}
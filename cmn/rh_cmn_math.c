




#include <stdio.h>
#include "rh_cmn_math.h"


u8 inline rh_cmn_math__numOf1_32bits( u32 value){
    value = ( value & 0x55555555 ) + ( (value >>1)  & 0x55555555 );
    value = ( value & 0x33333333 ) + ( (value >>2)  & 0x33333333 );
    value = ( value & 0x0f0f0f0f ) + ( (value >>4)  & 0x0f0f0f0f );
    value = ( value & 0x00ff00ff ) + ( (value >>8)  & 0x00ff00ff );
    value = ( value & 0x0000ffff ) + ( (value >>16) & 0x0000ffff );
/* Alternative Method */
#if 0
    value = value - ((value >> 1) & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
    value = value + (value >> 8);
    value = value + (value >> 16);
#endif
    return value;
}

u8 inline rh_cmn_math__numOf0_32bits( u32 value){
    return rh_cmn_math__numOf1_32bits(~value);
}

u8 inline rh_cmn_math__numOf1_8bits( u8 value){
    __builtin_popcount(value);
}

u8 inline rh_cmn_math__numOf0_8bits( u8 value){
    __builtin_popcount(~value);
}



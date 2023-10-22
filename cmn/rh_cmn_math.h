

#ifndef RH_CMN_MATH_H
#define RH_CMN_MATH_H

#include "rh_common.h"



u8 rh_cmn_math__numOf1_32bits( u32 value);
u8 rh_cmn_math__numOf0_32bits( u32 value);

u8 rh_cmn_math__numOf1_8bits( u8 value);
u8 rh_cmn_math__numOf0_8bits( u8 value);

u8 rh_cmn_math__zeller( u16 year, u8 month, u8 day);
u8 rh_cmn_math__is_valid_date( u16 year, u8 month, u8 day);
u8 rh_cmn_math__is_valid_time( u8 hour, u8 minute, u8 second);

i16 rh_cmn_math__abs_angle_hour( u8 hour, u8 minute, u8 second);
i16 rh_cmn_math__abs_angle_minute( u8 minute, u8 second);
i16 rh_cmn_math__abs_angle_second( u8 second);
#endif
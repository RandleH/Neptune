#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;


#define RH_MAX(a,b)   ((a)>(b)? (a) : (b))
#define RH_MIN(a,b)   ((a)<(b)? (a) : (b))


#if defined ( __GNUC__ ) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* GNU and ARM Compiler 6 compilers */
#ifndef RH_WEAK
#define RH_WEAK __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ || (__ARMCC_VERSION && (__ARMCC_VERSION >= 6010050)) */

#ifndef RH_API
#define RH_API
#endif

#ifndef RH_NORETURN
#define RH_NORETURN
#endif

#ifndef RH_PERIODIC
#define RH_PERIODIC
#endif


#ifndef RH_ONETIME
#define RH_ONETIME
#endif


#ifndef OK
#define OK     (0)
#endif


#endif


#if !defined(ENGINE_TIME)
#define ENGINE_TIME

#include "engine/api/types.h"

#define ENGINE_TIME_MILLIS (u64)1000
#define ENGINE_TIME_MICROS (u64)1000000
#define ENGINE_TIME_NANOS  (u64)1000000000

u64  engine_time_get_precision(void);
u64  engine_time_get_ticks(void);
void engine_time_wait_idle(u64 start_ticks, u64 duration, u64 precision);
void engine_time_wait_sleep(u64 start_ticks, u64 duration, u64 precision);

#endif // ENGINE_TIME

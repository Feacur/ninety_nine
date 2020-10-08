#include "engine/api/code.h"
#include "engine/api/maths.h"

#include <Windows.h>

//
static u64 engine_time_precision;

//
// API
//

#include "engine/api/platform_time.h"

u64 engine_time_get_precision(void) {
	return engine_time_precision;
}

u64 engine_time_get_ticks(void) {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return (u64)value.QuadPart;
}

void engine_time_wait_idle(u64 start_ticks, u64 duration, u64 precision) {
	u64 duration_ticks = mul_div_u64(duration, engine_time_precision, precision);
	u64 frame_end_ticks = start_ticks + duration_ticks;
	u64 current_ticks;
	while (true) {
		current_ticks = (u64)engine_time_get_ticks();
		if (current_ticks >= frame_end_ticks) { break; }
		YieldProcessor();
	}
}

void engine_time_wait_sleep(u64 start_ticks, u64 duration, u64 precision) {
	u64 duration_ticks = mul_div_u64(duration, engine_time_precision, precision);
	u64 frame_end_ticks = start_ticks + duration_ticks;
	u64 current_ticks;
	while (true) {
		current_ticks = (u64)engine_time_get_ticks();
		if (current_ticks >= frame_end_ticks) { break; }
		u64 sleep_millis = mul_div_u64(frame_end_ticks - current_ticks, ENGINE_TIME_MILLIS, engine_time_precision);
		Sleep((DWORD)sleep_millis);
	}
}

//
// system API
//

#include "interoperations/system__time.h"

void engine_system__time_init(void) {
	LARGE_INTEGER performance_frequency;
	QueryPerformanceFrequency(&performance_frequency);
	engine_time_precision = (u64)performance_frequency.QuadPart;
}

void engine_system__time_deinit(void) {
}

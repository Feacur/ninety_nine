#include "engine/api/code.h"

#include <Windows.h>

//
// API
//

#include "engine/api/platform_time.h"

void engine_time_wait(void) {
	YieldProcessor();
}

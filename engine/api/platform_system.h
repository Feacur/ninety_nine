#if !defined(ENGINE_PLATFORM_SYSTEM)
#define ENGINE_PLATFORM_SYSTEM

#include "engine/api/primitive_types.h"

extern bool engine_system_should_close;
void engine_system_init(void);
void engine_system_deinit(void);
void engine_system_poll_events(void);

#endif // ENGINE_PLATFORM_SYSTEM

#if !defined(ENGINE_SYSTEM)
#define ENGINE_SYSTEM

#include "engine/api/types.h"

extern bool engine_system_should_close;
void engine_system_init(void);
void engine_system_poll_events(void);

#endif // ENGINE_SYSTEM

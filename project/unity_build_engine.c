#include "engine/internal/dummy.c"

#if defined(_WIN64) || defined(_WIN32)
#include "engine/platform_windows/ogl_context.c"
#include "engine/platform_windows/system.c"
#include "engine/platform_windows/time.c"
#include "engine/platform_windows/window.c"
#include "engine/platform_windows/file.c"
#else
#error "unknown platform"
#endif // platform

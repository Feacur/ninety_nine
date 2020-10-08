#include "engine/internal/maths.c"

#if defined(_WIN64) || defined(_WIN32)
#include "engine/platform_windows/opengl_context.c"
#include "engine/platform_windows/opengl_library.c"
#include "engine/platform_windows/system.c"
#include "engine/platform_windows/time.c"
#include "engine/platform_windows/window.c"
#include "engine/platform_windows/file.c"
#else
#error "unknown platform"
#endif // platform

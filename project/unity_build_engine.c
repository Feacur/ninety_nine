#include "engine/internal/dummy.c"

#if defined(_WIN64) || defined(_WIN32)
#include "engine/platform_windows/system.c"
#include "engine/platform_windows/file.c"
#else
#error "unknown platform"
#endif // platform

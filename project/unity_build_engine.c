#include "engine/internal/maths.c"
#include "engine/internal/opengl/opengl.c"
#include "engine/internal/opengl/rendering_vm.c"

#if defined(_WIN64) || defined(_WIN32)
#include "engine/platform_windows/platform_file.c"
#include "engine/platform_windows/platform_window.c"
#include "engine/platform_windows/platform_time.c"
#include "engine/platform_windows/platform_system.c"
#include "engine/platform_windows/opengl/rendering_context.c"
#include "engine/platform_windows/opengl/rendering_library.c"
#else
#error "unknown platform"
#endif // platform

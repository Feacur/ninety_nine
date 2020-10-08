#if !defined(ENGINE_WGL_WRAPPER)
#define ENGINE_WGL_WRAPPER

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <GL/wgl.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // ENGINE_WGL_WRAPPER

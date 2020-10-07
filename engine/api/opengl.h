#if !defined(ENGINE_OPENGL)
#define ENGINE_OPENGL

// https://www.khronos.org/opengl/

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <KHR/khrplatform.h>
#include <gl/glcorearb.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // ENGINE_OPENGL

#if !defined(ENGINE_OPENGL)
#define ENGINE_OPENGL

// https://www.khronos.org/opengl/
// https://www.khronos.org/registry/OpenGL/index_gl.php

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <KHR/khrplatform.h>
#include <GL/glcorearb.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

extern PFNGLGETINTEGERVPROC glGetIntegerv;
extern PFNGLGETSTRINGIPROC  glGetStringi;

extern PFNGLFINISHPROC glFinish;
extern PFNGLFLUSHPROC  glFlush;

#endif // ENGINE_OPENGL

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

#define REGISTRY_OPENGL(type, name) extern type gl ## name;
#include "engine/registry/opengl.h"

#endif // ENGINE_OPENGL

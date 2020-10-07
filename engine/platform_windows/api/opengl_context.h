#if !defined(ENGINE_OPENGL_CONTEXT)
#define ENGINE_OPENGL_CONTEXT

#include <Windows.h>

HGLRC engine_opengl_context_create(HDC hdc);
void  engine_opengl_context_destroy(HGLRC hglrc);

#endif // ENGINE_OPENGL_CONTEXT

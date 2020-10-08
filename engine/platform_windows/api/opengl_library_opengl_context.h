#if !defined(ENGINE_OPENGL_LIBRARY_OPENGL_CONTEXT)
#define ENGINE_OPENGL_LIBRARY_OPENGL_CONTEXT

#include "engine/api/types.h"
#include "wgl.h"

bool engine_opengl_context_has_arb(cstring name);
bool engine_opengl_context_has_ext(cstring name);

// DLL
HGLRC engine_CreateContext(HDC hDc);
BOOL  engine_DeleteContext(HGLRC oldContext);
PROC  engine_GetProcAddress(LPCSTR lpszProc);
BOOL  engine_MakeCurrent(HDC hDc, HGLRC newContext);
BOOL  engine_ShareLists(HGLRC hrcSrvShare, HGLRC hrcSrvSource);
// ARB
const char * engine_GetExtensionsStringARB(HDC hdc);
BOOL         engine_GetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
HGLRC        engine_CreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);
// EXT
const char * engine_GetExtensionsStringEXT(void);
BOOL         engine_SwapIntervalEXT(int interval);

#endif // ENGINE_OPENGL_LIBRARY_OPENGL_CONTEXT

#if !defined(ENGINE_WGL)
#define ENGINE_WGL

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <GL/wgl.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// DLL
typedef PFNWGLCREATECONTEXTPROC  CreateContext_func;
typedef PFNWGLDELETECONTEXTPROC  DeleteContext_func;
typedef PFNWGLGETPROCADDRESSPROC GetProcAddress_func;
typedef PFNWGLMAKECURRENTPROC    MakeCurrent_func;
typedef PFNWGLSHARELISTSPROC     ShareLists_func;
// ARB
typedef PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB_func;
typedef PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB_func;
typedef PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB_func;
// EXT
typedef PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT_func;
typedef PFNWGLSWAPINTERVALEXTPROC        SwapIntervalEXT_func;

#endif // ENGINE_WGL

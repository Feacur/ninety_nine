#if !defined(ENGINE_WGL_TINY)
#define ENGINE_WGL_TINY

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <KHR/khrplatform.h>
#include <gl/glcorearb.h>
#include <gl/wgl.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// DLL
typedef PFNWGLCREATECONTEXTPROC  CreateContext_func;
typedef PFNWGLDELETECONTEXTPROC  DeleteContext_func;
typedef PFNWGLGETPROCADDRESSPROC GetProcAddress_func;
typedef PFNWGLMAKECURRENTPROC    MakeCurrent_func;
typedef PFNWGLSHARELISTSPROC     ShareLists_func;

// EXT
typedef PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT_func;
typedef PFNWGLSWAPINTERVALEXTPROC        SwapIntervalEXT_func;

// ARB
typedef PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB_func;
typedef PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB_func;
typedef PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB_func;

#endif // ENGINE_WGL_TINY

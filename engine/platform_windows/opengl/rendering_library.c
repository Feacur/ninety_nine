#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/graphics_types.h"
#include "engine/api/opengl.h"

#include "../api/internal_window.h"

#include "wgl_wrapper.h"

// https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
// https://www.khronos.org/opengl/wiki/Multisampling

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

//
#define ENGINE_rendering_library_NAME "opengl32.dll"

struct Rendering_Library {
	HMODULE handle;

	struct {
		// DLL
		PFNWGLGETPROCADDRESSPROC GetProcAddress;
		PFNWGLCREATECONTEXTPROC  CreateContext;
		PFNWGLDELETECONTEXTPROC  DeleteContext;
		PFNWGLMAKECURRENTPROC    MakeCurrent;
		PFNWGLSHARELISTSPROC     ShareLists;
		// ARB
		PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB;
		PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
		PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB;
		// EXT
		PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT;
		PFNWGLSWAPINTERVALEXTPROC        SwapIntervalEXT;
	} wgl;

	cstring extensions_ext;
	cstring extensions_arb;
};
static struct Rendering_Library * instance;

static void impl_handle_extensions_loading(struct Rendering_Library * opengl, HDC hdc);
static bool impl_contains_full_word(cstring container, cstring value);
static void * impl_get_function(cstring name);

// void * engine_opengl_get_function(cstring name) {
// 	if (!name) { return NULL; }
// 
// 	PROC ogl_address = instance->wgl.GetProcAddress(name);
// 	if (ogl_address) { return (void *)ogl_address; }
// 
// 	FARPROC dll_address = GetProcAddress(instance->library, name);
// 	if (dll_address) { return (void *)dll_address; }
// 
// 	return NULL;
// }

//
// system API
//

#include "../interoperations/system__rendering_library.h"

void engine_system__rendering_library_load(void) {
	struct Rendering_Library * rendering_library = ENGINE_MALLOC(sizeof(*rendering_library));
	memset(rendering_library, 0, sizeof(*rendering_library));

	rendering_library->handle = LoadLibraryA(ENGINE_rendering_library_NAME);

	//
	rendering_library->wgl.GetProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(rendering_library->handle, "wglGetProcAddress");
	rendering_library->wgl.CreateContext  = (PFNWGLCREATECONTEXTPROC) GetProcAddress(rendering_library->handle, "wglCreateContext");
	rendering_library->wgl.DeleteContext  = (PFNWGLDELETECONTEXTPROC) GetProcAddress(rendering_library->handle, "wglDeleteContext");
	rendering_library->wgl.MakeCurrent    = (PFNWGLMAKECURRENTPROC)   GetProcAddress(rendering_library->handle, "wglMakeCurrent");
	rendering_library->wgl.ShareLists     = (PFNWGLSHARELISTSPROC)    GetProcAddress(rendering_library->handle, "wglShareLists");

	//
	HWND hwnd = CreateWindowExA(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		ENGINE_WINDOW_CLASS_NAME, "",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 1, 1,
		HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL
	);
	HDC hdc = GetDC(hwnd);

	impl_handle_extensions_loading(rendering_library, hdc);

	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

	//
	instance = rendering_library;
}

void engine_system__rendering_library_unload(void) {
	FreeLibrary(instance->handle);
	ENGINE_FREE(instance);
}

//
// context API
//

#include "library_context.h"

void engine_load_functions(void) {
	glGetIntegerv = (PFNGLGETINTEGERVPROC)impl_get_function("glGetIntegerv");
	glGetStringi  = (PFNGLGETSTRINGIPROC) impl_get_function("glGetStringi");
}

bool engine_has_arb(cstring name) {
	return impl_contains_full_word(instance->extensions_arb, name);
}

bool engine_has_ext(cstring name) {
	return impl_contains_full_word(instance->extensions_ext, name);
}

HGLRC engine_CreateContext(HDC hDc) { return instance->wgl.CreateContext(hDc); }
BOOL  engine_DeleteContext(HGLRC oldContext) { return instance->wgl.DeleteContext(oldContext); }
PROC  engine_GetProcAddress(LPCSTR lpszProc) { return instance->wgl.GetProcAddress(lpszProc); }
BOOL  engine_MakeCurrent(HDC hDc, HGLRC newContext) { return instance->wgl.MakeCurrent(hDc, newContext); }
BOOL  engine_ShareLists(HGLRC hrcSrvShare, HGLRC hrcSrvSource) { return instance->wgl.ShareLists(hrcSrvShare, hrcSrvSource); }

const char * engine_GetExtensionsStringARB(HDC hdc) { return instance->wgl.GetExtensionsStringARB(hdc); }
BOOL         engine_GetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues) { return instance->wgl.GetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues); }
HGLRC        engine_CreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList) { return instance->wgl.CreateContextAttribsARB(hDC, hShareContext, attribList); }

const char * engine_GetExtensionsStringEXT(void) { return instance->wgl.GetExtensionsStringEXT(); }
BOOL         engine_SwapIntervalEXT(int interval) { return instance->wgl.SwapIntervalEXT(interval); }

//
// internal implementaion, system
//

static void impl_handle_extensions_loading(struct Rendering_Library * rendering_library, HDC hdc) {
	PIXELFORMATDESCRIPTOR const pfd_hint = {
		.nSize        = sizeof(pfd_hint),
		.nVersion     = 1,
		.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType   = PFD_TYPE_RGBA,
		.iLayerType   = PFD_MAIN_PLANE,
		.cColorBits   = 32,
		.cDepthBits   = 24,
		.cStencilBits = 8,
	};

	int pfd_id = ChoosePixelFormat(hdc, &pfd_hint);
	if (!pfd_id) { ENGINE_DEBUG_BREAK(); return; }

	PIXELFORMATDESCRIPTOR pfd;
	int formats_count = DescribePixelFormat(hdc, pfd_id, sizeof(pfd), &pfd);
	if (!formats_count) { ENGINE_DEBUG_BREAK(); return; }

	BOOL pfd_found = SetPixelFormat(hdc, pfd_id, &pfd);
	if (!pfd_found) { ENGINE_DEBUG_BREAK(); return; }

	HGLRC hglrc = rendering_library->wgl.CreateContext(hdc);
	if (!hglrc) { ENGINE_DEBUG_BREAK(); return; }

	if (!rendering_library->wgl.MakeCurrent(hdc, hglrc)) { ENGINE_DEBUG_BREAK(); }
	else {
		rendering_library->wgl.GetExtensionsStringARB    = (PFNWGLGETEXTENSIONSSTRINGARBPROC)   rendering_library->wgl.GetProcAddress("wglGetExtensionsStringARB");
		rendering_library->wgl.GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)rendering_library->wgl.GetProcAddress("wglGetPixelFormatAttribivARB");
		rendering_library->wgl.CreateContextAttribsARB   = (PFNWGLCREATECONTEXTATTRIBSARBPROC)  rendering_library->wgl.GetProcAddress("wglCreateContextAttribsARB");
		rendering_library->wgl.GetExtensionsStringEXT    = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)   rendering_library->wgl.GetProcAddress("wglGetExtensionsStringEXT");
		rendering_library->wgl.SwapIntervalEXT           = (PFNWGLSWAPINTERVALEXTPROC)          rendering_library->wgl.GetProcAddress("wglSwapIntervalEXT");

		rendering_library->extensions_ext = rendering_library->wgl.GetExtensionsStringEXT();
		rendering_library->extensions_arb = rendering_library->wgl.GetExtensionsStringARB(hdc);
	}

	rendering_library->wgl.MakeCurrent(NULL, NULL);
	rendering_library->wgl.DeleteContext(hglrc);
}

//
// internal implementation
//

static bool impl_contains_full_word(cstring container, cstring value) {
	if (!container) { return false; }
	if (!value)     { return false; }

	size_t value_size = strlen(value);

	for (cstring start = container, end = container; *start; start = end) {
		while (*start == ' ') { ++start; }
		end = strchr(start, ' '); if (!end) { end = container + strlen(container); }

		if ((size_t)(end - start) != value_size) { continue; }
		if (strncmp(start, value, value_size)) { return true; }
	}

	return false;
}

static void * impl_get_function(cstring name) {
	if (!name) { return NULL; }

	PROC ogl_address = instance->wgl.GetProcAddress(name);
	if (ogl_address) { return (void *)ogl_address; }

	FARPROC dll_address = GetProcAddress(instance->handle, name);
	if (dll_address) { return (void *)dll_address; }

	return NULL;
}

//
#undef ENGINE_rendering_library_NAME

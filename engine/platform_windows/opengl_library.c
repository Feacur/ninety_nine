#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/graphics_types.h"
#include "engine/api/opengl.h"

#include "api/window_internal.h"
#include "api/wgl.h"

#include <Windows.h>

// https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
// https://www.khronos.org/opengl/wiki/Multisampling

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

//
#define ENGINE_OPENGL_LIBRARY_NAME "opengl32.dll"

struct Opengl_Library {
	HMODULE library;

	struct {
		// DLL
		GetProcAddress_func GetProcAddress;
		CreateContext_func  CreateContext;
		DeleteContext_func  DeleteContext;
		MakeCurrent_func    MakeCurrent;
		ShareLists_func     ShareLists;
		// ARB
		GetExtensionsStringARB_func    GetExtensionsStringARB;
		GetPixelFormatAttribivARB_func GetPixelFormatAttribivARB;
		CreateContextAttribsARB_func   CreateContextAttribsARB;
		// EXT
		GetExtensionsStringEXT_func GetExtensionsStringEXT;
		SwapIntervalEXT_func        SwapIntervalEXT;
	} wgl;

	cstring extensions_ext;
	cstring extensions_arb;
};
static struct Opengl_Library * instance;

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC hdc);
static bool impl_contains_full_word(cstring container, cstring value);

//
// API
//

#include "engine/api/platform_opengl_library.h"

void * engine_opengl_get_function(cstring name) {
	if (!name) { return NULL; }

	PROC ogl_address = engine_GetProcAddress(name);
	if (ogl_address) { return (void *)ogl_address; }

	FARPROC dll_address = GetProcAddress(instance->library, name);
	if (dll_address) { return (void *)dll_address; }

	return NULL;
}

//
// system API
//

#include "api/opengl_system.h"

void engine_opengl_system_init(void) {
	struct Opengl_Library * opengl = ENGINE_MALLOC(sizeof(*opengl));
	opengl->library = LoadLibraryA(ENGINE_OPENGL_LIBRARY_NAME);

	//
	#define LOAD_FUNCTION(name) opengl->wgl.name = (name ## _func)GetProcAddress(opengl->library, "wgl" # name)
	LOAD_FUNCTION(GetProcAddress);
	LOAD_FUNCTION(CreateContext);
	LOAD_FUNCTION(DeleteContext);
	LOAD_FUNCTION(MakeCurrent);
	LOAD_FUNCTION(ShareLists);
	#undef LOAD_FUNCTION

	//
	HWND hwnd = CreateWindowExA(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		ENGINE_WINDOW_CLASS_NAME, "",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 1, 1,
		HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL
	);
	HDC hdc = GetDC(hwnd);

	impl_handle_extensions_loading(opengl, hdc);

	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

	//
	instance = opengl;
}

void engine_opengl_system_deinit(void) {
	FreeLibrary(instance->library);
	ENGINE_FREE(instance);
}

//
// context API
//

#include "api/opengl_library_opengl_context.h"

bool engine_opengl_context_has_arb(cstring name) {
	return impl_contains_full_word(instance->extensions_arb, name);
}

bool engine_opengl_context_has_ext(cstring name) {
	return impl_contains_full_word(instance->extensions_ext, name);
}

// DLL
HGLRC engine_CreateContext(HDC hDc) { return instance->wgl.CreateContext(hDc); }
BOOL  engine_DeleteContext(HGLRC oldContext) { return instance->wgl.DeleteContext(oldContext); }
PROC  engine_GetProcAddress(LPCSTR lpszProc) { return instance->wgl.GetProcAddress(lpszProc); }
BOOL  engine_MakeCurrent(HDC hDc, HGLRC newContext) { return instance->wgl.MakeCurrent(hDc, newContext); }
BOOL  engine_ShareLists(HGLRC hrcSrvShare, HGLRC hrcSrvSource) { return instance->wgl.ShareLists(hrcSrvShare, hrcSrvSource); }
// ARB
const char * engine_GetExtensionsStringARB(HDC hdc) { return instance->wgl.GetExtensionsStringARB(hdc); }
BOOL         engine_GetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues) { return instance->wgl.GetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues); }
HGLRC        engine_CreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList) { return instance->wgl.CreateContextAttribsARB(hDC, hShareContext, attribList); }
// EXT
const char * engine_GetExtensionsStringEXT(void) { return instance->wgl.GetExtensionsStringEXT(); }
BOOL         engine_SwapIntervalEXT(int interval) { return instance->wgl.SwapIntervalEXT(interval); }

//
// internal implementaion, system
//

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC hdc) {
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

	HGLRC hrc = opengl->wgl.CreateContext(hdc);
	if (!hrc) { ENGINE_DEBUG_BREAK(); return; }

	if (!opengl->wgl.MakeCurrent(hdc, hrc)) { ENGINE_DEBUG_BREAK(); }
	else {
		#define LOAD_FUNCTION(name) opengl->wgl.name = (name ## _func)opengl->wgl.GetProcAddress("wgl" # name)
		LOAD_FUNCTION(GetExtensionsStringARB);
		LOAD_FUNCTION(GetPixelFormatAttribivARB);
		LOAD_FUNCTION(CreateContextAttribsARB);
		LOAD_FUNCTION(GetExtensionsStringEXT);
		LOAD_FUNCTION(SwapIntervalEXT);
		#undef LOAD_FUNCTION

		opengl->extensions_ext = opengl->wgl.GetExtensionsStringEXT();
		opengl->extensions_arb = opengl->wgl.GetExtensionsStringARB(hdc);
	}

	opengl->wgl.MakeCurrent(NULL, NULL);
	opengl->wgl.DeleteContext(hrc);
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

//
#undef ENGINE_OPENGL_LIBRARY_NAME

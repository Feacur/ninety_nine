#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/graphics_types.h"
#include "api/window_context.h"
#include "api/window_internal.h"

#include <Windows.h>
#include "api/wgl_tiny.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include "glad/glad.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

//
#define ENGINE_OPENGL_LIBRARY_NAME "opengl32.dll"

struct Settings_OGL hint_settings_ogl;
struct Pixel_Format hint_pixel_format;

struct Opengl_Library {
	HMODULE library;

	// WGL functions
	CreateContext_func  * CreateContext;
	DeleteContext_func  * DeleteContext;
	GetProcAddress_func * GetProcAddress;
	MakeCurrent_func    * MakeCurrent;

	// ARB functions
	CreateContextAttribsARB_func   * CreateContextAttribsARB;
	GetPixelFormatAttribivARB_func * GetPixelFormatAttribivARB;
};
static struct Opengl_Library * instance;

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC hdc);

//
// API
//

#include "engine/api/platform_opengl.h"

void * engine_opengl_get_function(cstring name) {
	if (!name) { return NULL; }

	PROC ogl_address = instance->GetProcAddress(name);
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
	// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
	struct Opengl_Library * opengl = ENGINE_MALLOC(sizeof(*opengl));
	opengl->library = LoadLibraryA(ENGINE_OPENGL_LIBRARY_NAME);

	//
	#define LOAD_FUNCTION(name) opengl->name = (name ## _func *)GetProcAddress(opengl->library, "wgl" # name)
	LOAD_FUNCTION(GetProcAddress);
	LOAD_FUNCTION(CreateContext);
	LOAD_FUNCTION(DeleteContext);
	LOAD_FUNCTION(MakeCurrent);
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
	gladLoadGLLoader(engine_opengl_get_function);
}

void engine_opengl_system_deinit(void) {
	FreeLibrary(instance->library);
	ENGINE_FREE(instance);
}

//
// context API
//

#include "api/opengl_context.h"

HGLRC engine_opengl_context_create(HDC hdc) {
	(void)hdc;
	return NULL;
}

void engine_opengl_context_destroy(HGLRC hrc) {
	(void)hrc;
}

// bool engine_opengl_has_extension(cstring container, cstring value) {
// 	cstring start = container;
// 	cstring end   = container + strlen(container);

// 	while (true) {
// 		cstring from = strstr(start, value);
// 		if (!from) { break; }

// 		cstring to = from + strlen(value);
// 		if (to > end) { break; }

// 		if (from == start || *(from - 1) == ' ') {
// 			if (*to == ' ')  { return true; }
// 			if (*to == '\0') { return true; }
// 		}

// 		start = to;
// 	}

// 	return false;
// }

//
// internal implementaion, system
//

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC hdc) {
	PIXELFORMATDESCRIPTOR pfd = {
		.nSize        = sizeof(pfd),
		.nVersion     = 1,
		.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType   = PFD_TYPE_RGBA,
		.iLayerType   = PFD_MAIN_PLANE,
		.cColorBits   = 32,
		.cDepthBits   = 24,
		.cStencilBits = 8,
	};

	int pfd_id = ChoosePixelFormat(hdc, &pfd);
	if (!pfd_id) { ENGINE_DEBUG_BREAK(); return; }
	if (!SetPixelFormat(hdc, pfd_id, &pfd)) { ENGINE_DEBUG_BREAK(); return; }

	HGLRC hrc = opengl->CreateContext(hdc);
	if (!hrc) { ENGINE_DEBUG_BREAK(); return; }

	if (!opengl->MakeCurrent(hdc, hrc)) {
		opengl->DeleteContext(hrc);
		ENGINE_DEBUG_BREAK(); return;
	}

	#define LOAD_FUNCTION(name) opengl->name = (name ## _func *)opengl->GetProcAddress("wgl" # name)
	LOAD_FUNCTION(CreateContextAttribsARB);
	LOAD_FUNCTION(GetPixelFormatAttribivARB);
	#undef LOAD_FUNCTION

	opengl->MakeCurrent(NULL, NULL);
	opengl->DeleteContext(hrc);
}

//
#undef ENGINE_OPENGL_LIBRARY_NAME

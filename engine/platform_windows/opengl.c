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

	struct {
		// DLL
		CreateContext_func  * CreateContext;
		DeleteContext_func  * DeleteContext;
		GetProcAddress_func * GetProcAddress;
		MakeCurrent_func    * MakeCurrent;
		ShareLists_func     * ShareLists;
		// ARB
		CreateContextAttribsARB_func   * CreateContextAttribsARB;
		GetPixelFormatAttribivARB_func * GetPixelFormatAttribivARB;
	} wgl;

	cstring extensions_ext;
	cstring extensions_arb;
};
static struct Opengl_Library * instance;

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC device_context);
static bool impl_contains_full_word(cstring container, cstring value);

//
// API
//

#include "engine/api/platform_opengl.h"

void * engine_opengl_get_function(cstring name) {
	if (!name) { return NULL; }

	PROC ogl_address = instance->wgl.GetProcAddress(name);
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
	#define LOAD_FUNCTION(name) opengl->wgl.name = (name ## _func *)GetProcAddress(opengl->library, "wgl" # name)
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

HGLRC engine_opengl_context_create(HDC device_context) {
	HGLRC const shared = NULL;
	HGLRC hglrc;

	bool ARB_create_context = impl_contains_full_word(instance->extensions_arb, "WGL_ARB_create_context");

	if (ARB_create_context && instance->wgl.CreateContext) {
		int * attributes = NULL;
		hglrc = instance->wgl.CreateContextAttribsARB(device_context, shared, attributes);
	}
	else {
		hglrc = instance->wgl.CreateContext(device_context);
		if (shared) { instance->wgl.ShareLists(shared, hglrc); }
	}
	return hglrc;
}

void engine_opengl_context_destroy(HGLRC handle) {
	instance->wgl.DeleteContext(handle);
}

//
// internal implementaion, system
//

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC device_context) {
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

	int pfd_id = ChoosePixelFormat(device_context, &pfd);
	if (!pfd_id) { ENGINE_DEBUG_BREAK(); return; }

	BOOL pfd_found = SetPixelFormat(device_context, pfd_id, &pfd);
	if (!pfd_found) { ENGINE_DEBUG_BREAK(); return; }

	HGLRC hrc = opengl->wgl.CreateContext(device_context);
	if (!hrc) { ENGINE_DEBUG_BREAK(); return; }

	if (!opengl->wgl.MakeCurrent(device_context, hrc)) { ENGINE_DEBUG_BREAK(); }
	else {
		#define LOAD_FUNCTION(name) opengl->wgl.name = (name ## _func *)opengl->wgl.GetProcAddress("wgl" # name)
		LOAD_FUNCTION(CreateContextAttribsARB);
		LOAD_FUNCTION(GetPixelFormatAttribivARB);
		#undef LOAD_FUNCTION

		#define LOAD_FUNCTION(name) name ## _func * wgl ## name = (name ## _func *)opengl->wgl.GetProcAddress("wgl" # name)
		LOAD_FUNCTION(GetExtensionsStringEXT);
		LOAD_FUNCTION(GetExtensionsStringARB);
		#undef LOAD_FUNCTION

		opengl->extensions_ext = wglGetExtensionsStringEXT();
		opengl->extensions_arb = wglGetExtensionsStringARB(device_context);
	}

	opengl->wgl.MakeCurrent(NULL, NULL);
	opengl->wgl.DeleteContext(hrc);
}

static bool impl_contains_full_word(cstring container, cstring value) {
	if (!container) { return false; }
	if (!value) {     return false; }

	size_t container_size = strlen(container);
	size_t value_size     = strlen(value);

	cstring start = container;
	while (*start) {
		while (*start == ' ') { ++start; }
		cstring end = strchr(start, ' ');
		if (!end) { end = container + container_size; }

		if ((size_t)(end - start) == value_size && strncmp(start, value, value_size)) { return true; }

		start = end;
	}

	return false;
}

//
#undef ENGINE_OPENGL_LIBRARY_NAME

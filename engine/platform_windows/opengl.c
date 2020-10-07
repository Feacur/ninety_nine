#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/graphics_types.h"
#include "api/window_context.h"
#include "api/window_internal.h"

#include <Windows.h>

#include "api/wgl.h"

// https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
// https://www.khronos.org/opengl/wiki/Multisampling

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

//
#define ENGINE_OPENGL_LIBRARY_NAME "opengl32.dll"

struct Settings_CTX hint_settings_ctx;

struct Opengl_Library {
	HMODULE library;

	struct {
		// DLL
		CreateContext_func  CreateContext;
		DeleteContext_func  DeleteContext;
		GetProcAddress_func GetProcAddress;
		MakeCurrent_func    MakeCurrent;
		ShareLists_func     ShareLists;
		// ARB
		CreateContextAttribsARB_func   CreateContextAttribsARB;
		GetPixelFormatAttribivARB_func GetPixelFormatAttribivARB;
	} wgl;

	cstring extensions_ext;
	cstring extensions_arb;
};
static struct Opengl_Library * instance;

struct Pixel_Format {
	int id;
	int double_buffer, swap_method;
	int r, g, b, a;
	int depth, stencil;
	int samples, sample_buffers;
};

static void impl_handle_extensions_loading(struct Opengl_Library * opengl, HDC hdc);
static bool impl_contains_full_word(cstring container, cstring value);
static struct Pixel_Format * impl_allocate_pixel_formats(HDC hdc);
static struct Pixel_Format * impl_allocate_pixel_formats_legacy(HDC hdc);
static int impl_choose_pixel_format(struct Pixel_Format const * formats);
static HGLRC impl_create_context(HDC hdc, HGLRC shared);
static HGLRC impl_create_context_legacy(HDC hdc, HGLRC shared);

#define HAS_EXT(name) impl_contains_full_word(instance->extensions_ext, # name)
#define HAS_ARB(name) impl_contains_full_word(instance->extensions_arb, # name)

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
	struct Opengl_Library * opengl = ENGINE_MALLOC(sizeof(*opengl));
	opengl->library = LoadLibraryA(ENGINE_OPENGL_LIBRARY_NAME);

	//
	#define LOAD_FUNCTION(name) opengl->wgl.name = (name ## _func)GetProcAddress(opengl->library, "wgl" # name)
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
}

void engine_opengl_system_deinit(void) {
	FreeLibrary(instance->library);
	ENGINE_FREE(instance);
}

//
// context API
//

#include "api/opengl_context.h"

#define FORCE_LEGACY false

HGLRC engine_opengl_context_create(HDC hdc) {
	HGLRC const shared = NULL;

	struct Pixel_Format * formats = impl_allocate_pixel_formats(hdc);
	if (!formats) { formats = impl_allocate_pixel_formats_legacy(hdc); }

	int pfd_id = impl_choose_pixel_format(formats);
	if (!pfd_id) { ENGINE_DEBUG_BREAK(); return NULL; }

	PIXELFORMATDESCRIPTOR pfd;
	int formats_count = DescribePixelFormat(hdc, pfd_id, sizeof(pfd), &pfd);
	if (!formats_count) { ENGINE_DEBUG_BREAK(); return NULL; }

	BOOL pfd_found = SetPixelFormat(hdc, pfd_id, &pfd);
	if (!pfd_found) { ENGINE_DEBUG_BREAK(); return NULL; }

	HGLRC result = impl_create_context(hdc, shared);
	if (!result) { result = impl_create_context_legacy(hdc, shared); }
	return result;
}

void engine_opengl_context_destroy(HGLRC hglrc) {
	instance->wgl.DeleteContext(hglrc);
}

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
		LOAD_FUNCTION(CreateContextAttribsARB);
		LOAD_FUNCTION(GetPixelFormatAttribivARB);
		#undef LOAD_FUNCTION

		#define LOAD_FUNCTION(name) name ## _func wgl ## name = (name ## _func)opengl->wgl.GetProcAddress("wgl" # name)
		LOAD_FUNCTION(GetExtensionsStringEXT);
		LOAD_FUNCTION(GetExtensionsStringARB);
		#undef LOAD_FUNCTION

		opengl->extensions_ext = wglGetExtensionsStringEXT();
		opengl->extensions_arb = wglGetExtensionsStringARB(hdc);
	}

	opengl->wgl.MakeCurrent(NULL, NULL);
	opengl->wgl.DeleteContext(hrc);
}

//
// internal implementation, context
//

static int impl_get_pfd_value(int const * keys, int * vals, int key) {
	for (int i = 0; keys[i]; ++i) {
		if (keys[i] == key) { return vals[i]; }
	}
	return 0;
}

static struct Pixel_Format * impl_allocate_pixel_formats(HDC hdc) {
	// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
	if (!HAS_ARB(WGL_ARB_pixel_format)) { return 0; }

	int const formats_request = WGL_NUMBER_PIXEL_FORMATS_ARB; int formats_capacity;
	if (!instance->wgl.GetPixelFormatAttribivARB(hdc, 0, 0, 1, &formats_request, &formats_capacity)) { ENGINE_DEBUG_BREAK(); return 0; }
	if (!formats_capacity) { ENGINE_DEBUG_BREAK(); return 0; }

	int const request_keys[] = {
		WGL_DRAW_TO_WINDOW_ARB,
		WGL_ACCELERATION_ARB,
		WGL_SWAP_METHOD_ARB,
		WGL_SUPPORT_GDI_ARB,
		WGL_SUPPORT_OPENGL_ARB,
		WGL_DOUBLE_BUFFER_ARB,
		WGL_PIXEL_TYPE_ARB,
		//
		WGL_RED_BITS_ARB,
		WGL_GREEN_BITS_ARB,
		WGL_BLUE_BITS_ARB,
		WGL_ALPHA_BITS_ARB,
		WGL_DEPTH_BITS_ARB,
		WGL_STENCIL_BITS_ARB,
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_multisample.txt
		WGL_SAMPLES_ARB,
		WGL_SAMPLE_BUFFERS_ARB,
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_framebuffer_sRGB.txt
		WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,
	};
	int request_vals[sizeof(request_keys) / sizeof(request_keys[0])];
	int const attributes_number = sizeof(request_keys) / sizeof(request_keys[0]);

	int formats_count = 0;
	struct Pixel_Format * formats = ENGINE_MALLOC((size_t)(formats_capacity + 1) * sizeof(struct Pixel_Format));
	for (int i = 0; i < formats_capacity; ++i) {
		int pfd_id = i + 1;
		if (!instance->wgl.GetPixelFormatAttribivARB(hdc, pfd_id, 0, attributes_number, request_keys, request_vals)) { ENGINE_DEBUG_BREAK(); continue; }

		#define GET_PFD_VALUE(key) impl_get_pfd_value(request_keys, request_vals, key)

		if (GET_PFD_VALUE(WGL_DRAW_TO_WINDOW_ARB) == false)                     { continue; }
		if (GET_PFD_VALUE(WGL_SUPPORT_GDI_ARB)    == true)                      { continue; }
		if (GET_PFD_VALUE(WGL_SUPPORT_OPENGL_ARB) == false)                     { continue; }
		if (GET_PFD_VALUE(WGL_ACCELERATION_ARB)   != WGL_FULL_ACCELERATION_ARB) { continue; }
		if (GET_PFD_VALUE(WGL_PIXEL_TYPE_ARB)     != WGL_TYPE_RGBA_ARB)         { continue; }

		if (HAS_ARB(ARB_multisample)) {
			if (!GET_PFD_VALUE(WGL_SAMPLES_ARB))        { continue; }
			if (!GET_PFD_VALUE(WGL_SAMPLE_BUFFERS_ARB)) { continue; }
		}

		if (HAS_ARB(ARB_framebuffer_sRGB)) {
			if (!GET_PFD_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) { continue; }
		}

		formats[formats_count++] = (struct Pixel_Format){
			.id = pfd_id,
			.double_buffer = GET_PFD_VALUE(WGL_DOUBLE_BUFFER_ARB),
			.swap_method   = GET_PFD_VALUE(WGL_SWAP_METHOD_ARB) == WGL_SWAP_EXCHANGE_ARB ? 1
			               : GET_PFD_VALUE(WGL_SWAP_METHOD_ARB) == WGL_SWAP_COPY_ARB     ? 2
			               :                                                               0,
			.r = GET_PFD_VALUE(WGL_RED_BITS_ARB),
			.g = GET_PFD_VALUE(WGL_GREEN_BITS_ARB),
			.b = GET_PFD_VALUE(WGL_BLUE_BITS_ARB),
			.a = GET_PFD_VALUE(WGL_ALPHA_BITS_ARB),
			.depth   = GET_PFD_VALUE(WGL_DEPTH_BITS_ARB),
			.stencil = GET_PFD_VALUE(WGL_STENCIL_BITS_ARB),
			.samples        = GET_PFD_VALUE(WGL_SAMPLES_ARB),
			.sample_buffers = GET_PFD_VALUE(WGL_SAMPLE_BUFFERS_ARB),
		};

		#undef GET_PFD_VALUE
	}

	formats[formats_count].id = 0;
	return formats;
}

static struct Pixel_Format * impl_allocate_pixel_formats_legacy(HDC hdc) {
	int formats_capacity = DescribePixelFormat(hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL);
	if (!formats_capacity) { return 0; }

	int formats_count = 0;
	struct Pixel_Format * formats = ENGINE_MALLOC((size_t)(formats_capacity + 1) * sizeof(struct Pixel_Format));
	for (int i = 0; i < formats_capacity; ++i) {
		int pfd_id = i + 1;
		PIXELFORMATDESCRIPTOR pfd;
		if (!DescribePixelFormat(hdc, pfd_id, sizeof(pfd), &pfd)) { ENGINE_DEBUG_BREAK(); continue; }

		if ((pfd.dwFlags & PFD_DRAW_TO_WINDOW)      != PFD_DRAW_TO_WINDOW)      { continue; }
		if ((pfd.dwFlags & PFD_SUPPORT_GDI)         == PFD_SUPPORT_GDI)         { continue; }
		if ((pfd.dwFlags & PFD_SUPPORT_OPENGL)      != PFD_SUPPORT_OPENGL)      { continue; }
		if ((pfd.dwFlags & PFD_GENERIC_ACCELERATED) != PFD_GENERIC_ACCELERATED) { continue; }
		if ((pfd.dwFlags & PFD_GENERIC_FORMAT)      == PFD_GENERIC_FORMAT)      { continue; }
		if ((pfd.dwFlags & PFD_TYPE_RGBA)           == PFD_TYPE_RGBA)           { continue; }

		formats[formats_count++] = (struct Pixel_Format){
			.id = pfd_id,
			.double_buffer = (pfd.dwFlags & PFD_DOUBLEBUFFER) == PFD_DOUBLEBUFFER,
			.swap_method   = (pfd.dwFlags & PFD_SWAP_EXCHANGE) == PFD_SWAP_EXCHANGE ? 1
			               : (pfd.dwFlags & PFD_SWAP_COPY) == PFD_SWAP_COPY         ? 2
			               :                                                          0,
			.r = pfd.cRedBits,
			.g = pfd.cGreenBits,
			.b = pfd.cBlueBits,
			.a = pfd.cAlphaBits,
			.depth   = pfd.cDepthBits,
			.stencil = pfd.cStencilBits,
		};
	}

	formats[formats_count].id = 0;
	return formats;
}

static int impl_choose_pixel_format(struct Pixel_Format const * formats) {
	int result = 0;
	for (int i = 0; formats[i].id; ++i) {
		result = formats[i].id;
		break;
	}
	return result;
}

static HGLRC impl_create_context(HDC hdc, HGLRC shared) {
	// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
	if (!HAS_ARB(WGL_ARB_create_context)) { return NULL; }

	int attributes_count = 0;
	int attributes[8 * 2];
	int const attributes_capacity = sizeof(attributes) / sizeof(attributes[0]);

	int context_flags = 0;
	int context_profile_mask  = 0;

	#define ADD_ATTRIBUTE(key, value) do { attributes[attributes_count++] = key; attributes[attributes_count++] = value; } while(false)

	if (hint_settings_ctx.version) {
		ADD_ATTRIBUTE(WGL_CONTEXT_MAJOR_VERSION_ARB, hint_settings_ctx.version / 10);
		ADD_ATTRIBUTE(WGL_CONTEXT_MINOR_VERSION_ARB, hint_settings_ctx.version % 10);
	}

	bool const legacy = false;
	if (legacy) {
		context_profile_mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	}
	else {
		if (hint_settings_ctx.version >= 30) {
			context_profile_mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
			context_flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
	}

	bool flush_on_release = false;
	if (HAS_ARB(ARB_context_flush_control)) {
		// https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_context_flush_control.txt
		ADD_ATTRIBUTE(
			WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
			flush_on_release ? WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB : WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB
		);
	}

	bool const no_error = false;
	if (no_error && HAS_ARB(ARB_create_context_no_error)) {
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_create_context_no_error.txt
		ADD_ATTRIBUTE(WGL_CONTEXT_OPENGL_NO_ERROR_ARB, true);
	}
	else {
		int const robustness_mode = 0;
		if (robustness_mode && HAS_ARB(ARB_create_context_robustness)) {
			// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context_robustness.txt
			ADD_ATTRIBUTE(
				WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
				robustness_mode == 1 ? WGL_NO_RESET_NOTIFICATION_ARB : WGL_LOSE_CONTEXT_ON_RESET_ARB
			);
			context_flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
		}

		bool const context_debug = false;
		if (context_debug) { context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; }
	}

	if (context_flags) {
		ADD_ATTRIBUTE(WGL_CONTEXT_FLAGS_ARB, context_flags);
	}

	if (context_profile_mask && hint_settings_ctx.version >= 32) {
		ADD_ATTRIBUTE(WGL_CONTEXT_PROFILE_MASK_ARB, context_profile_mask);
	}

	ADD_ATTRIBUTE(0, 0);
	#undef ADD_ATTRIBUTE

	if (attributes_count > attributes_capacity) { ENGINE_DEBUG_BREAK(); }

	return instance->wgl.CreateContextAttribsARB(hdc, shared, attributes);
}

static HGLRC impl_create_context_legacy(HDC hdc, HGLRC shared) {
	HGLRC result = instance->wgl.CreateContext(hdc);
	if (shared) { instance->wgl.ShareLists(shared, result); }
	return result;
}

//
// internal implementation
//

static bool impl_contains_full_word(cstring container, cstring value) {
	if (!container) { return false; }
	if (!value) {     return false; }

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
#undef HAS_EXT
#undef HAS_ARB

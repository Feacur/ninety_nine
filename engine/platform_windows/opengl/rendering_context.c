#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/graphics_types.h"
#include "engine/internal/opengl/opengl.h"

#include "../interoperations/rendering_context__window.h"

#include "library_context.h"
#include "wgl_wrapper.h"

//
struct Settings_CTX hint_settings_ctx;

struct Pixel_Format {
	struct Settings_CTX settings;
	int id;
};

static HGLRC impl_create_context_auto(HDC hdc, struct Pixel_Format * format);

#define HAS_ARB(name) engine_has_arb(# name)
// #define HAS_EXT(name) engine_has_ext(# name)

//
// API
//

#include "../api/rendering_context.h"

struct Rendering_Context {
	HGLRC handle;
	struct Pixel_Format format;
	struct Engine_Window * window;
	HDC cached_device;
};

struct Rendering_Context * engine_rendering_context_create(struct Engine_Window * window) {
	struct Rendering_Context * context = ENGINE_MALLOC(sizeof(*context));
	memset(context, 0, sizeof(*context));
	context->window = window;

	HWND hwnd = engine_rendering_context__window_get_handle(window);
	context->cached_device = GetDC(hwnd);

	context->handle = impl_create_context_auto(context->cached_device, &context->format);
	engine_MakeCurrent(context->cached_device, context->handle);

	engine_load_functions();

	return context;
}

void engine_rendering_context_destroy(struct Rendering_Context * context) {
	engine_MakeCurrent(NULL, NULL);
	engine_DeleteContext(context->handle);

	HWND hwnd = engine_rendering_context__window_get_handle(context->window);
	ReleaseDC(hwnd, context->cached_device);

	engine_rendering_context__window_detach(context->window);
	ENGINE_FREE(context);
}

void engine_rendering_context_update(struct Rendering_Context * context) {
	if (!context->format.settings.buffering || !SwapBuffers(context->cached_device)) {
		// glFinish();
		glFlush();
	}
}

//
//
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
	if (!engine_GetPixelFormatAttribivARB(hdc, 0, 0, 1, &formats_request, &formats_capacity)) { ENGINE_DEBUG_BREAK(); return 0; }
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
		if (!engine_GetPixelFormatAttribivARB(hdc, pfd_id, 0, attributes_number, request_keys, request_vals)) { ENGINE_DEBUG_BREAK(); continue; }

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

		int swap_method = 0;
		switch (GET_PFD_VALUE(WGL_SWAP_METHOD_ARB)) {
			case WGL_SWAP_EXCHANGE_ARB:  swap_method = 1; break;
			case WGL_SWAP_COPY_ARB:      swap_method = 2; break;
			case WGL_SWAP_UNDEFINED_ARB: swap_method = 3; break;
		}

		formats[formats_count++] = (struct Pixel_Format){
			.id = pfd_id,
			.settings = {
				.buffering = GET_PFD_VALUE(WGL_DOUBLE_BUFFER_ARB),
				.swap_method = swap_method,
				.r = GET_PFD_VALUE(WGL_RED_BITS_ARB),
				.g = GET_PFD_VALUE(WGL_GREEN_BITS_ARB),
				.b = GET_PFD_VALUE(WGL_BLUE_BITS_ARB),
				.a = GET_PFD_VALUE(WGL_ALPHA_BITS_ARB),
				.depth   = GET_PFD_VALUE(WGL_DEPTH_BITS_ARB),
				.stencil = GET_PFD_VALUE(WGL_STENCIL_BITS_ARB),
				.samples        = GET_PFD_VALUE(WGL_SAMPLES_ARB),
				.sample_buffers = GET_PFD_VALUE(WGL_SAMPLE_BUFFERS_ARB),
			},
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

		int swap_method = 0;
		if (false) {}
		else if ((pfd.dwFlags & PFD_SWAP_EXCHANGE) == PFD_SWAP_EXCHANGE) { swap_method = 1; }
		else if ((pfd.dwFlags & PFD_SWAP_EXCHANGE) == PFD_SWAP_COPY)     { swap_method = 2; }
		else                                                             { swap_method = 3; }

		formats[formats_count++] = (struct Pixel_Format){
			.id = pfd_id,
			.settings = {
				.buffering = (pfd.dwFlags & PFD_DOUBLEBUFFER)  == PFD_DOUBLEBUFFER,
				.swap_method = swap_method,
				.r = pfd.cRedBits,
				.g = pfd.cGreenBits,
				.b = pfd.cBlueBits,
				.a = pfd.cAlphaBits,
				.depth   = pfd.cDepthBits,
				.stencil = pfd.cStencilBits,
			},
		};
	}

	formats[formats_count].id = 0;
	return formats;
}

static struct Pixel_Format impl_choose_pixel_format(struct Pixel_Format const * formats) {
	struct Pixel_Format const * format = formats;
	for (; format->id; ++format) {
		if (format->settings.r       < hint_settings_ctx.r)       { continue; }
		if (format->settings.g       < hint_settings_ctx.g)       { continue; }
		if (format->settings.b       < hint_settings_ctx.b)       { continue; }
		if (format->settings.a       < hint_settings_ctx.a)       { continue; }
		if (format->settings.depth   < hint_settings_ctx.depth)   { continue; }
		if (format->settings.stencil < hint_settings_ctx.stencil) { continue; }

		if (format->settings.buffering < hint_settings_ctx.buffering) { continue; }
		if (hint_settings_ctx.swap_method) {
			if (format->settings.swap_method != hint_settings_ctx.swap_method) { continue; }
		}

		if (format->settings.samples < hint_settings_ctx.samples) { continue; }
		if (format->settings.sample_buffers < hint_settings_ctx.sample_buffers) { continue; }

		break;
	}
	return *format;
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

	bool const support_legacy = false;
	if (support_legacy) {
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

	bool const context_no_error = false;
	if (context_no_error && HAS_ARB(ARB_create_context_no_error)) {
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_create_context_no_error.txt
		ADD_ATTRIBUTE(WGL_CONTEXT_OPENGL_NO_ERROR_ARB, true);
	}
	else {
		int const context_robustness_mode = 0;
		if (context_robustness_mode && HAS_ARB(ARB_create_context_robustness)) {
			// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context_robustness.txt
			ADD_ATTRIBUTE(
				WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
				context_robustness_mode == 1 ? WGL_NO_RESET_NOTIFICATION_ARB : WGL_LOSE_CONTEXT_ON_RESET_ARB
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

	return engine_CreateContextAttribsARB(hdc, shared, attributes);
}

static HGLRC impl_create_context_legacy(HDC hdc, HGLRC shared) {
	HGLRC result = engine_CreateContext(hdc);
	if (shared) { engine_ShareLists(shared, result); }
	return result;
}

static HGLRC impl_create_context_auto(HDC hdc, struct Pixel_Format * format) {
	HGLRC const shared = NULL;

	struct Pixel_Format * formats = impl_allocate_pixel_formats(hdc);
	if (!formats) { formats = impl_allocate_pixel_formats_legacy(hdc); }

	struct Pixel_Format pixel_format = impl_choose_pixel_format(formats);
	if (!pixel_format.id) { ENGINE_DEBUG_BREAK(); return NULL; }

	PIXELFORMATDESCRIPTOR pfd;
	int formats_count = DescribePixelFormat(hdc, pixel_format.id, sizeof(pfd), &pfd);
	if (!formats_count) { ENGINE_DEBUG_BREAK(); return NULL; }

	BOOL pfd_found = SetPixelFormat(hdc, pixel_format.id, &pfd);
	if (!pfd_found) { ENGINE_DEBUG_BREAK(); return NULL; }

	HGLRC result = impl_create_context(hdc, shared);
	if (!result) { result = impl_create_context_legacy(hdc, shared); }

	*format = pixel_format;
	return result;
}

//
#undef HAS_ARB
#undef HAS_EXT

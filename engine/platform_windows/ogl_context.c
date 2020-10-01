#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/platform_window.h"
#include "window_context.h"

#include <Windows.h>
#include "ogl_wgl_tiny.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include "glad/glad.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

//
#define ENGINE_OPENGL_LIBRARY_NAME "opengl32.dll"

struct Opengl_Library {
	HMODULE library;
};
static struct Opengl_Library * opengl;

//
// API
//

#include "ogl_context.h"

struct Rendering_Context_OGL {
	HDC   hdc;
	HGLRC hrc;
	u8    vsync;
};

struct Rendering_Context_OGL * engine_ogl_context_create(struct Engine_Window * window) {
	struct Rendering_Context_OGL * context = ENGINE_MALLOC(sizeof(*context));
	context->hdc = engine_window_get_hdc(window);
	return context;
}

void engine_ogl_context_destroy(struct Rendering_Context_OGL * context) {
	ENGINE_FREE(context);
}

//
// system API
//

#include "ogl_system.h"

void engine_system_init_opengl(void) {
	opengl = ENGINE_MALLOC(sizeof(*opengl));
	opengl->library = LoadLibraryA(ENGINE_OPENGL_LIBRARY_NAME);

}

void engine_system_deinit_opengl(void) {
	FreeLibrary(opengl->library);
	ENGINE_FREE(opengl);
}

//
#undef ENGINE_OPENGL_LIBRARY_NAME

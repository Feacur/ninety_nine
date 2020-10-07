#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/platform_opengl.h"
#include "api/window_context.h"
#include "api/opengl_context.h"

//
// API
//

#include "api/context.h"

struct Rendering_Context {
	HGLRC handle;
	struct Engine_Window * window;
};

struct Rendering_Context * engine_context_create(struct Engine_Window * window) {
	struct Rendering_Context * context = ENGINE_MALLOC(sizeof(*context));
	memset(context, 0, sizeof(*context));

	HWND hwnd = engine_window_context_get_handle(window);
	HDC  hdc  = GetDC(hwnd);

	context->handle = engine_opengl_context_create(hdc);
	context->window = window;

	ReleaseDC(hwnd, hdc);

	return context;
}

void engine_context_destroy(struct Rendering_Context * context) {
	engine_opengl_context_destroy(context->handle); context->handle = NULL;
	engine_window_context_detach(context->window); context->window = NULL;
	ENGINE_FREE(context);
}

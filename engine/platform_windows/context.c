#include "engine/api/code.h"
#include "engine/api/types.h"
#include "api/window_context.h"
#include "api/opengl_context.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include "glad/glad.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

//
// API
//

#include "api/context.h"

struct Rendering_Context {
	HDC   hdc;
	HGLRC hrc;
	u8    vsync;
};

struct Rendering_Context * engine_context_create(struct Engine_Window * window) {
	struct Rendering_Context * context = ENGINE_MALLOC(sizeof(*context));
	context->hdc = engine_window_get_hdc(window);
	context->hrc = engine_opengl_context_create(context->hdc);
	context->vsync = 0;
	return context;
}

void engine_context_destroy(struct Rendering_Context * context) {
	engine_opengl_context_destroy(context->hrc);
	context->hrc = NULL;
	ENGINE_FREE(context);
}

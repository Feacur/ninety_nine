#if !defined(ENGINE_OGL_CONTEXT)
#define ENGINE_OGL_CONTEXT

struct Engine_Window;

struct Rendering_Context_OGL;
struct Rendering_Context_OGL * engine_ogl_context_create(struct Engine_Window * window);
void engine_ogl_context_destroy(struct Rendering_Context_OGL * context);

#endif // ENGINE_OGL_CONTEXT

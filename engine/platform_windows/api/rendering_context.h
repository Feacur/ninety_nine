#if !defined(ENGINE_RENDERING_CONTEXT)
#define ENGINE_RENDERING_CONTEXT

struct Engine_Window;

struct Rendering_Context;
struct Rendering_Context * engine_rendering_context_create(struct Engine_Window * window);
void engine_rendering_context_destroy(struct Rendering_Context * context);

#endif // ENGINE_RENDERING_CONTEXT

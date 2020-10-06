#if !defined(ENGINE_CONTEXT)
#define ENGINE_CONTEXT

struct Engine_Window;

struct Rendering_Context;
struct Rendering_Context * engine_context_create(struct Engine_Window * window);
void engine_context_destroy(struct Rendering_Context * context);

#endif // ENGINE_CONTEXT

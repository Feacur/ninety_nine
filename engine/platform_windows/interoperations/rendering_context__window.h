#if !defined(ENGINE_WINDOW_CONTEXT)
#define ENGINE_WINDOW_CONTEXT

#include <Windows.h>

struct Engine_Window;
HWND engine_rendering_context__window_get_handle(struct Engine_Window * window);
void engine_rendering_context__window_detach(struct Engine_Window * context);

#endif // ENGINE_WINDOW_CONTEXT

#if !defined(ENGINE_WINDOW_CONTEXT)
#define ENGINE_WINDOW_CONTEXT

#include <Windows.h>

struct Engine_Window;
HDC engine_window_get_hdc(struct Engine_Window * window);

#endif // ENGINE_WINDOW_CONTEXT

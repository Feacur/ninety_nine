#if !defined(ENGINE_WINDOW)
#define ENGINE_WINDOW

struct Engine_Window;
struct Engine_Window * engine_window_create(void);
void engine_window_destroy(struct Engine_Window * window);
bool engine_window_is_active(struct Engine_Window * window);
void engine_window_init_context(struct Engine_Window * window);

void engine_window_toggle_borderless_fullsreen(struct Engine_Window * window);

#endif // ENGINE_WINDOW

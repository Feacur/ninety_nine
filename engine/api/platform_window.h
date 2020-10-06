#if !defined(ENGINE_WINDOW)
#define ENGINE_WINDOW

#include "engine/api/math_types.h"

struct Engine_Window;
struct Engine_Window * engine_window_create(void);
void engine_window_destroy(struct Engine_Window * window);
bool engine_window_is_active(struct Engine_Window * window);
void engine_window_init_context(struct Engine_Window * window);
void engine_window_deinit_context(struct Engine_Window * window);

void engine_window_update(struct Engine_Window * window);

svec2 engine_window_mouse_delta(struct Engine_Window * window);
svec2 engine_window_mouse_display_position(struct Engine_Window * window);
svec2 engine_window_mouse_window_position(struct Engine_Window * window);
bool  engine_window_mouse_button(struct Engine_Window * window, u8 button);
bool  engine_window_mouse_transition(struct Engine_Window * window, u8 button, bool state);

bool engine_window_key(struct Engine_Window * window, u8 key);
bool engine_window_key_transition(struct Engine_Window * window, u8 key, bool state);

void engine_window_toggle_raw_input(struct Engine_Window * window);
void engine_window_toggle_borderless_fullsreen(struct Engine_Window * window);

u16 engine_window_get_refresh_rate(struct Engine_Window * window, u16 default_value);

#endif // ENGINE_WINDOW

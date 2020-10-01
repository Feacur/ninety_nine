#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/dummy.h"
#include "engine/api/key_codes.h"
#include "engine/api/platform_system.h"
#include "engine/api/platform_time.h"
#include "engine/api/platform_window.h"
#include "engine/api/platform_file.h"

int main(int argc, char * argv[]) {
	(void)argc; (void)argv;
	engine_system_init();

	dummy();

	u8 * buffer = NULL; size_t buffer_size = 0;
	engine_file_read("assets/settings.cfg", &buffer, &buffer_size);
	printf("%.*s\n", (u32)buffer_size, buffer);

	printf("file time: %zd\n", engine_file_time("assets/settings.cfg"));

	free(buffer);

	struct Engine_Window * window = engine_window_create();
	engine_window_init_context(window);
	while (!engine_system_should_close && window && engine_window_is_active(window)) {
		engine_window_update(window);
		engine_system_poll_events();
		engine_time_wait();

		if (engine_window_key_transition(window, KC_Tab, true)) {
			engine_window_toggle_raw_input(window);
		}

		if (engine_window_key(window, KC_Alt) && engine_window_key_transition(window, KC_Enter, true)) {
			engine_window_toggle_borderless_fullsreen(window);
		}

		if (engine_window_key(window, KC_Alt) && engine_window_key_transition(window, KC_F4, true)) { break; }
	}
	if (window) { engine_window_destroy(window); }
	engine_system_deinit();
}

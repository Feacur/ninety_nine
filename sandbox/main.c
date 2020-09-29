#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/system.h"
#include "engine/api/window.h"
#include "engine/api/file.h"
#include "engine/api/dummy.h"

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
	while (!engine_system_should_close) {
		if (!window) { break; }
		if (!engine_window_is_active(window)) { break; }
		engine_system_poll_events();
		YieldProcessor();
	}
	if (window) { engine_window_destroy(window); }
	engine_system_deinit();
}

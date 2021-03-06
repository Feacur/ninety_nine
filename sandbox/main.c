#include "engine/api/code.h"
#include "engine/api/graphics_types.h"
#include "engine/api/maths.h"
#include "engine/api/key_codes.h"
#include "engine/api/rendering_vm.h"
#include "engine/api/platform_system.h"
#include "engine/api/platform_time.h"
#include "engine/api/platform_window.h"
#include "engine/api/platform_file.h"

int main(int argc, char * argv[]) {
	(void)argc; (void)argv;

	//
	hint_settings_ctx = (struct Settings_CTX){
		.version = 46,
		.buffering = true, .swap_method = 1,
		.r = 8, .g = 8, .b = 8, .a = 8,
		.depth = 24, .stencil = 8,
	};
	engine_system_init();

	//
	cstring asset_path_shader = "assets/shaders/texture_tint.glsl";
	u8 * buffer = NULL; size_t buffer_size = 0;
	engine_file_read(asset_path_shader, &buffer, &buffer_size);
	printf("%.*s\n", (u32)buffer_size, buffer);

	printf("file time: %zd\n", engine_file_time(asset_path_shader));

	ENGINE_FREE(buffer);

	//
	size_t rendering_buffer_length = 0;
	size_t rendering_buffer_capacity = 1024;
	u8 * rendering_buffer = ENGINE_MALLOC(rendering_buffer_capacity);

	//
	u64 start_ticks = engine_time_get_ticks();
	struct Engine_Window * window = engine_window_create();
	engine_window_toggle_raw_input(window);
	engine_window_init_context(window);
	while (!engine_system_should_close && window && engine_window_is_active(window)) {
		// update OS
		engine_window_update(window);
		engine_system_poll_events();
		if (!engine_window_is_active(window)) { break; }

		// wait the frame ends
		u16 refresh_rate = engine_window_get_refresh_rate(window, 144);
		engine_time_wait_idle(start_ticks, ENGINE_TIME_NANOS / refresh_rate, ENGINE_TIME_NANOS);

		//
		u64 current_ticks = engine_time_get_ticks();
		u64 frame_ticks = current_ticks - start_ticks;
		start_ticks = current_ticks;

		r32 dt = (r32)frame_ticks / (r32)(engine_time_get_precision());

		// update logic
		(void)dt;
		engine_rendering_vm_update(rendering_buffer, rendering_buffer_length);

		// process system input
		if (engine_window_key(window, KC_Alt) && engine_window_key_transition(window, KC_F4, true)) { break; }

		if (engine_window_key(window, KC_Alt) && engine_window_key_transition(window, KC_Enter, true)) {
			engine_window_toggle_borderless_fullsreen(window);
		}

		if (engine_window_key_transition(window, KC_F12, true)) {
			engine_window_toggle_raw_input(window);
		}
	}
	if (window) { engine_window_destroy(window); }

	//
	engine_system_deinit();
}

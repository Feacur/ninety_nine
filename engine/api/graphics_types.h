#if !defined(ENGINE_GRAPHICS_TYPES)
#define ENGINE_GRAPHICS_TYPES

struct Settings_CTX {
	int version;
	int buffering, swap_method;
	int r, g, b, a;
	int depth, stencil;
	int samples, sample_buffers;
};

extern struct Settings_CTX hint_settings_ctx;

#endif // ENGINE_GRAPHICS_TYPES

#if !defined(ENGINE_GRAPHICS_TYPES)
#define ENGINE_GRAPHICS_TYPES

struct Settings_CTX {
	int version;
	int buffering;
	int r, g, b, a;
	int depth, stencil;
};

extern struct Settings_CTX hint_settings_ctx;

#endif // ENGINE_GRAPHICS_TYPES

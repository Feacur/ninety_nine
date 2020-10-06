#if !defined(ENGINE_GRAPHICS_TYPES)
#define ENGINE_GRAPHICS_TYPES

struct Settings_OGL {
	int major, minor;
};

struct Pixel_Format {
	int r, g, b, a;
	int depth, stencil;
};

extern struct Settings_OGL hint_settings_ogl;
extern struct Pixel_Format hint_pixel_format;

#endif // ENGINE_GRAPHICS_TYPES

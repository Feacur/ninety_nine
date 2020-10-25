#if !defined(ENGINE_GRAPHICS_TYPES)
#define ENGINE_GRAPHICS_TYPES

struct Settings_CTX {
	int version;
	int buffering, swap_method;
	int r, g, b, a;
	int depth, stencil;
	int samples, sample_buffers;
};

enum Data_Type {
	#define REGISTRY_DATA_TYPE(name) Data_Type_ ## name,
	#include "engine/registry/data_type.h"
};

enum Texture_Type {
	Texture_Type_Color,
	Texture_Type_Depth,
	Texture_Type_Stencil,
	Texture_Type_DStencil,
};

enum Filter_Type {
	Filter_Type_None,
	Filter_Type_Point,
	Filter_Type_Linear,
};

enum Wrap_Type {
	Wrap_Type_Clamp,
	Wrap_Type_Repeat,
	Wrap_Type_MClamp,
	Wrap_Type_MRepeat,
};

enum Mesh_Frequency {
	Mesh_Frequency_Static,
	Mesh_Frequency_Dynamic,
	Mesh_Frequency_Stream,
};

enum Mesh_Access {
	Mesh_Access_Draw,
	Mesh_Access_Read,
	Mesh_Access_Copy,
};

extern struct Settings_CTX hint_settings_ctx;

#endif // ENGINE_GRAPHICS_TYPES

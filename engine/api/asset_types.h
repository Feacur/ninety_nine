#if !defined(ENGINE_ASSET_TYPES)
#define ENGINE_ASSET_TYPES

#include "engine/api/math_types.h"
#include "engine/api/graphics_types.h"

struct Asset_Shader {
	u8 * data; size_t length;
};

struct Asset_Texture {
	u8 * data; size_t length;
	enum Data_Type type; u8 channels;
	enum Texture_Type kind;
	enum Filter_Type filter_mipmap, filter_min, filter_max;
	enum Wrap_Type wrap_x, wrap_y;
};

struct Asset_Mesh {
	u8 * data; size_t length;
	enum Data_Type type;
	enum Mesh_Frequency frequency;
	enum Mesh_Access access;
};

#endif // ENGINE_ASSET_TYPES

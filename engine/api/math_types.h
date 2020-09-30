#if !defined(ENGINE_MATH_TYPES)
#define ENGINE_MATH_TYPES

#include "engine/api/types.h"

typedef struct vec2 { r32 x, y; } vec2;
typedef struct vec3 { r32 x, y, z; } vec3;
typedef struct vec4 { r32 x, y, z, w; } vec4;

typedef struct svec2 { s32 x, y; } svec2;
typedef struct svec3 { s32 x, y, z; } svec3;
typedef struct svec4 { s32 x, y, z, w; } svec4;

typedef struct uvec2 { u32 x, y; } uvec2;
typedef struct uvec3 { u32 x, y, z; } uvec3;
typedef struct uvec4 { u32 x, y, z, w; } uvec4;

typedef struct mat2 { vec3 x, y; } mat2;
typedef struct mat3 { vec2 x, y, z; } mat3;
typedef struct mat4 { vec4 x, y, z, w; } mat4;

#endif // ENGINE_MATH_TYPES

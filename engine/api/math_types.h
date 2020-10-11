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

typedef vec2 cplx;
typedef vec4 quat;

//
#define VEC2(x, y) (vec2){x, y}
#define VEC3(x, y, z) (vec3){x, y, z}
#define VEC4(x, y, z, w) (vec4){x, y, z, w}

#define SVEC2(x, y) (svec2){x, y}
#define SVEC3(x, y, z) (svec3){x, y, z}
#define SVEC4(x, y, z, w) (svec4){x, y, z, w}

#define UVEC2(x, y) (uvec2){x, y}
#define UVEC3(x, y, z) (uvec3){x, y, z}
#define UVEC4(x, y, z, w) (uvec4){x, y, z, w}

#define MAT2(x, y) (mat2){x, y}
#define MAT3(x, y, z) (mat3){x, y, z}
#define MAT4(x, y, z, w) (mat4){x, y, z, w}

#define CPLX(x, y) (cplx){x, y}
#define QUAT(x, y, z, w) (cplx){x, y, z, w}

#endif // ENGINE_MATH_TYPES

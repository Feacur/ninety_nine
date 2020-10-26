#if !defined(ENGINE_MATH_TYPES)
#define ENGINE_MATH_TYPES

#include "engine/api/primitive_types.h"

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
#define QUAT(x, y, z, w) (quat){x, y, z, w}

//
#define VEC2_SINGLE(v) (vec2){v, v}
#define VEC3_SINGLE(v) (vec3){v, v, v}
#define VEC4_SINGLE(v) (vec4){v, v, v, v}

#define SVEC2_SINGLE(v) (svec2){v, v}
#define SVEC3_SINGLE(v) (svec3){v, v, v}
#define SVEC4_SINGLE(v) (svec4){v, v, v, v}

#define UVEC2_SINGLE(v) (uvec2){v, v}
#define UVEC3_SINGLE(v) (uvec3){v, v, v}
#define UVEC4_SINGLE(v) (uvec4){v, v, v, v}

//
#define CPLX_IDENTITY_ROTATION() CPLX(1,0)
#define QUAT_IDENTITY_ROTATION() QUAT(0,0,0,1)

#endif // ENGINE_MATH_TYPES

#if !defined(ENGINE_MATHS)
#define ENGINE_MATHS

#include "engine/api/math_types.h"

s32 mul_div_s32(s32 value, s32 numerator, s32 denominator);
u64 mul_div_u64(u64 value, u64 numerator, u64 denominator);

r32 lerp(r32 v1, r32 v2, r32 t);

r32 min_r32(r32 v1, r32 v2);
s32 min_s32(s32 v1, s32 v2);
u32 min_u32(u32 v1, u32 v2);

r32 max_r32(r32 v1, r32 v2);
s32 max_s32(s32 v1, s32 v2);
u32 max_u32(u32 v1, u32 v2);

r32 clamp_r32(r32 v, r32 low, r32 high);
s32 clamp_s32(s32 v, s32 low, s32 high);
u32 clamp_u32(u32 v, u32 low, u32 high);

// vec2
vec2 vec2_add(vec2 v1, vec2 v2);
vec2 vec2_sub(vec2 v1, vec2 v2);
vec2 vec2_mul(vec2 v1, vec2 v2);
vec2 vec2_div(vec2 v1, vec2 v2);
r32 vec2_dot(vec2 v1, vec2 v2);

svec2 svec2_add(svec2 v1, svec2 v2);
svec2 svec2_sub(svec2 v1, svec2 v2);
svec2 svec2_mul(svec2 v1, svec2 v2);
svec2 svec2_div(svec2 v1, svec2 v2);
s32 svec2_dot(svec2 v1, svec2 v2);

uvec2 uvec2_add(uvec2 v1, uvec2 v2);
uvec2 uvec2_sub(uvec2 v1, uvec2 v2);
uvec2 uvec2_mul(uvec2 v1, uvec2 v2);
uvec2 uvec2_div(uvec2 v1, uvec2 v2);
u32 uvec2_dot(uvec2 v1, uvec2 v2);

// vec3
vec3 vec3_add(vec3 v1, vec3 v2);
vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_mul(vec3 v1, vec3 v2);
vec3 vec3_div(vec3 v1, vec3 v2);
r32 vec3_dot(vec3 v1, vec3 v2);

svec3 svec3_add(svec3 v1, svec3 v2);
svec3 svec3_sub(svec3 v1, svec3 v2);
svec3 svec3_mul(svec3 v1, svec3 v2);
svec3 svec3_div(svec3 v1, svec3 v2);
s32 svec3_dot(svec3 v1, svec3 v2);

uvec3 uvec3_add(uvec3 v1, uvec3 v2);
uvec3 uvec3_sub(uvec3 v1, uvec3 v2);
uvec3 uvec3_mul(uvec3 v1, uvec3 v2);
uvec3 uvec3_div(uvec3 v1, uvec3 v2);
u32 uvec3_dot(uvec3 v1, uvec3 v2);

// vec4
vec4 vec4_add(vec4 v1, vec4 v2);
vec4 vec4_sub(vec4 v1, vec4 v2);
vec4 vec4_mul(vec4 v1, vec4 v2);
vec4 vec4_div(vec4 v1, vec4 v2);
r32 vec4_dot(vec4 v1, vec4 v2);

svec4 svec4_add(svec4 v1, svec4 v2);
svec4 svec4_sub(svec4 v1, svec4 v2);
svec4 svec4_mul(svec4 v1, svec4 v2);
svec4 svec4_div(svec4 v1, svec4 v2);
s32 svec4_dot(svec4 v1, svec4 v2);

uvec4 uvec4_add(uvec4 v1, uvec4 v2);
uvec4 uvec4_sub(uvec4 v1, uvec4 v2);
uvec4 uvec4_mul(uvec4 v1, uvec4 v2);
uvec4 uvec4_div(uvec4 v1, uvec4 v2);
u32 uvec4_dot(uvec4 v1, uvec4 v2);

// vec3, cross product
vec3 vec3_cross(vec3 v1, vec3 v2);

// vec4, quaternion
quat quat_set_axis(vec3 axis, r32 radians);
quat quat_set_radians(vec3 radians);
quat quat_conjugate(quat q);
quat quat_reciprocal(quat q);
quat quat_mul(quat q1, quat q2);
vec3 quat_transform(quat q, vec3 v);
void quat_get_axes(quat q, vec3 * x, vec3 * y, vec3 * z);

// mat4
mat4 mat4_set_transformation(vec3 position, vec3 scale, quat rotation);
mat4 mat4_set_projection(vec2 scale, r32 ncp, r32 fcp, r32 ortho);
mat4 mat4_inverse_transformation(mat4 m);
vec4 mat4_transform_vec(mat4 m, vec4 v);
mat4 mat4_transform_mat(mat4 m1, mat4 m2);

#endif // ENGINE_MATHS

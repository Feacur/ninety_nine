#include "engine/api/code.h"
#include "engine/api/maths.h"

#include <math.h>

s32 mul_div_s32(s32 value, s32 numerator, s32 denominator) {
	s32 a = value / denominator; s32 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}

u64 mul_div_u64(u64 value, u64 numerator, u64 denominator) {
	u64 a = value / denominator; u64 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}

r32 lerp(r32 v1, r32 v2, r32 t) { return v1*(1 - t) + v2*t; }

/*
left-handed coordinate system

> basis vectors multiplication
ij = -ji = k
jk = -kj = i
ki = -ik = j

> generic vector multiplication
(x1*i + y1*j + z1*k + w1) * (x2*i + y2*j + z2*k + w2)

  x1*i * (x2*i + y2*j + z2*k + w2) = x1*x2*(ii) + x1*y2*(ij) + x1*z2*(ik) + x1*w2*(i )
+ y1*j * (x2*i + y2*j + z2*k + w2) = y1*x2*(ji) + y1*y2*(jj) + y1*z2*(jk) + y1*w2*(j )
+ z1*k * (x2*i + y2*j + z2*k + w2) = z1*x2*(ki) + z1*y2*(kj) + z1*z2*(kk) + z1*w2*(k )
+ w1   * (x2*i + y2*j + z2*k + w2) = w1*x2*( i) + w1*y2*( j) + w1*z2*( k) + w1*w2*(  )

> cross product
ii = jj = kk = ijk =  0 == sin(0)

> dot product
ii = jj = kk = ijk =  1 == cos(0)

> quaternion
ii = jj = kk = ijk = -1 == cos(180)
*/

// vec2
vec2 vec2_add(vec2 v1, vec2 v2) { return (vec2){v1.x + v2.x, v1.y + v2.y}; }
vec2 vec2_sub(vec2 v1, vec2 v2) { return (vec2){v1.x - v2.x, v1.y - v2.y}; }
vec2 vec2_mul(vec2 v1, vec2 v2) { return (vec2){v1.x * v2.x, v1.y * v2.y}; }
vec2 vec2_div(vec2 v1, vec2 v2) { return (vec2){v1.x / v2.x, v1.y / v2.y}; }
r32 vec2_dot(vec2 v1, vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }

svec2 svec2_add(svec2 v1, svec2 v2) { return (svec2){v1.x + v2.x, v1.y + v2.y}; }
svec2 svec2_sub(svec2 v1, svec2 v2) { return (svec2){v1.x - v2.x, v1.y - v2.y}; }
svec2 svec2_mul(svec2 v1, svec2 v2) { return (svec2){v1.x * v2.x, v1.y * v2.y}; }
svec2 svec2_div(svec2 v1, svec2 v2) { return (svec2){v1.x / v2.x, v1.y / v2.y}; }
s32 svec2_dot(svec2 v1, svec2 v2) { return v1.x * v2.x + v1.y * v2.y; }

uvec2 uvec2_add(uvec2 v1, uvec2 v2) { return (uvec2){v1.x + v2.x, v1.y + v2.y}; }
uvec2 uvec2_sub(uvec2 v1, uvec2 v2) { return (uvec2){v1.x - v2.x, v1.y - v2.y}; }
uvec2 uvec2_mul(uvec2 v1, uvec2 v2) { return (uvec2){v1.x * v2.x, v1.y * v2.y}; }
uvec2 uvec2_div(uvec2 v1, uvec2 v2) { return (uvec2){v1.x / v2.x, v1.y / v2.y}; }
u32 uvec2_dot(uvec2 v1, uvec2 v2) { return v1.x * v2.x + v1.y * v2.y; }

// vec3
vec3 vec3_add(vec3 v1, vec3 v2) { return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
vec3 vec3_sub(vec3 v1, vec3 v2) { return (vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
vec3 vec3_mul(vec3 v1, vec3 v2) { return (vec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
vec3 vec3_div(vec3 v1, vec3 v2) { return (vec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
r32 vec3_dot(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

svec3 svec3_add(svec3 v1, svec3 v2) { return (svec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
svec3 svec3_sub(svec3 v1, svec3 v2) { return (svec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
svec3 svec3_mul(svec3 v1, svec3 v2) { return (svec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
svec3 svec3_div(svec3 v1, svec3 v2) { return (svec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
s32 svec3_dot(svec3 v1, svec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

uvec3 uvec3_add(uvec3 v1, uvec3 v2) { return (uvec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
uvec3 uvec3_sub(uvec3 v1, uvec3 v2) { return (uvec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
uvec3 uvec3_mul(uvec3 v1, uvec3 v2) { return (uvec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
uvec3 uvec3_div(uvec3 v1, uvec3 v2) { return (uvec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
u32 uvec3_dot(uvec3 v1, uvec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// vec4
vec4 vec4_add(vec4 v1, vec4 v2) { return (vec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w}; }
vec4 vec4_sub(vec4 v1, vec4 v2) { return (vec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w}; }
vec4 vec4_mul(vec4 v1, vec4 v2) { return (vec4){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w}; }
vec4 vec4_div(vec4 v1, vec4 v2) { return (vec4){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w}; }
r32 vec4_dot(vec4 v1, vec4 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }

svec4 svec4_add(svec4 v1, svec4 v2) { return (svec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w}; }
svec4 svec4_sub(svec4 v1, svec4 v2) { return (svec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w}; }
svec4 svec4_mul(svec4 v1, svec4 v2) { return (svec4){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w}; }
svec4 svec4_div(svec4 v1, svec4 v2) { return (svec4){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w}; }
s32 svec4_dot(svec4 v1, svec4 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }

uvec4 uvec4_add(uvec4 v1, uvec4 v2) { return (uvec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w}; }
uvec4 uvec4_sub(uvec4 v1, uvec4 v2) { return (uvec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w}; }
uvec4 uvec4_mul(uvec4 v1, uvec4 v2) { return (uvec4){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w}; }
uvec4 uvec4_div(uvec4 v1, uvec4 v2) { return (uvec4){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w}; }
u32 uvec4_dot(uvec4 v1, uvec4 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }

// vec3, cross product
vec3 vec3_cross(vec3 v1, vec3 v2) {
	return (vec3){
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x,
	};
}

// vec4, quaternion
// > assumed to be normalized (of unit length)
quat quat_set_axis(vec3 axis, r32 radians) {
	r32 const r = radians * 0.5f;
	r32 const s = sinf(r);
	r32 const c = cosf(r);
	return (quat){axis.x * s, axis.y * s, axis.z * s, c};
}

quat quat_set_radians(vec3 radians) {
	// quat_set_axis(VEC3(0,1,0), radians.y) * quat_set_axis(VEC3(1,0,0), radians.x) * quat_set_axis(VEC3(0,0,1), radians.z)
	vec3 const r = vec3_mul(radians, VEC3(0.5f, 0.5f, 0.5f));
	vec3 const s = VEC3(sinf(r.x), sinf(r.y), sinf(r.z));
	vec3 const c = VEC3(cosf(r.x), cosf(r.y), cosf(r.z));
	r32 const sy_cx = s.y*c.x; r32 const cy_sx = c.y*s.x;
	r32 const cy_cx = c.y*c.x; r32 const sy_sx = s.y*s.x;
	return (quat){
		sy_cx*s.z + cy_sx*c.z,
		sy_cx*c.z - cy_sx*s.z,
		cy_cx*s.z - sy_sx*c.z,
		cy_cx*c.z + sy_sx*s.z,
	};
}

quat quat_conjugate(quat q) { return (quat){-q.x, -q.y, -q.z, q.w}; }
quat quat_reciprocal(quat q) {
	r32 ms = vec4_dot(q, q);
	return vec4_div(quat_conjugate(q), VEC4(ms, ms, ms, ms));
}

quat quat_mul(quat q1, quat q2) {
	return (quat){
		 q1.x*q2.w + q1.y*q2.z - q1.z*q2.y + q1.w*q2.x,
		-q1.x*q2.z + q1.y*q2.w + q1.z*q2.x + q1.w*q2.y,
		 q1.x*q2.y - q1.y*q2.x + q1.z*q2.w + q1.w*q2.z,
		-q1.x*q2.x - q1.y*q2.y - q1.z*q2.z + q1.w*q2.w,
	};
}

vec3 quat_transform(quat q, vec3 v) {
	// formula: q * (quat){v.x, v.y, v.z, 0} * quat_reciprocal(q)
	// if q is unit, quat_reciprocal == quat_conjugate
	r32 const xx = q.x*q.x; r32 const yy = q.y*q.y; r32 const zz = q.z*q.z; r32 const ww = q.w*q.w;
	r32 const xy = q.x*q.y; r32 const yz = q.y*q.z; r32 const zw = q.z*q.w; r32 const wx = q.w*q.x;
	r32 const xz = q.x*q.z; r32 const yw = q.y*q.w;
	return (vec3){
		v.x * ( xx - yy - zz + ww) + (v.y * (xy - zw) + v.z * (yw + xz)) * 2,
		v.y * (-xx + yy - zz + ww) + (v.x * (zw + xy) + v.z * (yz - wx)) * 2,
		v.z * (-xx - yy + ww + zz) + (v.x * (xz - yw) + v.y * (yz + wx)) * 2,
	};
}

// void quat_get_axes(quat q, vec3 * x, vec3 * y, vec3 * z);
// void quat_get_radians(quat q, vec3 * radians);

// mat4

/*
> parameters
- XYZ: world space
- XYZ': normalized space

> orthograhic
- XYZ' = (offset_z + scale_z * XYZ) / 1
- XY scale: from [-bounds .. bounds] to [-1 .. 1]
- Z  scale: from [Znear .. Zfar]     to [NCP .. 1]

> perspective
- XYZ' = (offset_z + scale_z * XYZ) / Z
- XY scale; from [-bounds .. bounds] to [-1 .. 1]
- Z  scale; from [Znear .. Zfar]     to [NCP .. 1]
*/

mat4 mat4_projection(vec2 scale, r32 ncp, r32 fcp, r32 ortho) {
	r32 const NNCP = 0; // r32 const NFCP = 1;
	r32 const reverse_depth = 1 / (fcp - ncp);

	r32 const persp_scale_z  = isinf(fcp) ? 1                  : (reverse_depth * (fcp - NNCP * ncp));
	r32 const persp_offset_z = isinf(fcp) ? ((NNCP - 1) * ncp) : (reverse_depth * (NNCP - 1) * ncp * fcp);

	r32 const ortho_scale_z  = isinf(fcp) ? 0 : (reverse_depth * (1 - NNCP));
	r32 const ortho_offset_z = isinf(fcp) ? 0 : (reverse_depth * (fcp * NNCP - ncp));

	r32 const scale_z  = lerp(persp_scale_z, ortho_scale_z, ortho);
	r32 const offset_z = lerp(persp_offset_z, ortho_offset_z, ortho);
	r32 const zw = 1 - ortho;
	r32 const ww = ortho;

	return (mat4){
		{scale.x, 0, 0,  0},
		{0, scale.y, 0,  0},
		{0, 0, scale_z,  zw},
		{0, 0, offset_z, ww},
	};
}

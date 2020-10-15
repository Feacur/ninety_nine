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

r32 min_r32(r32 v1, r32 v2) { return (v1 < v2) ? v1 : v2; }
s32 min_s32(s32 v1, s32 v2) { return (v1 < v2) ? v1 : v2; }
u32 min_u32(u32 v1, u32 v2) { return (v1 < v2) ? v1 : v2; }

r32 max_r32(r32 v1, r32 v2) { return (v1 > v2) ? v1 : v2; }
s32 max_s32(s32 v1, s32 v2) { return (v1 > v2) ? v1 : v2; }
u32 max_u32(u32 v1, u32 v2) { return (v1 > v2) ? v1 : v2; }

r32 clamp_r32(r32 v, r32 low, r32 high) { return min_r32(max_r32(v, low), high); }
s32 clamp_s32(s32 v, s32 low, s32 high) { return min_s32(max_s32(v, low), high); }
u32 clamp_u32(u32 v, u32 low, u32 high) { return min_u32(max_u32(v, low), high); }

// vec2

/*
2D
left-handed coordinate system

> generic vector multiplication
result = (x1 + y1 * i) * (x2 + y2 * i)
result = x1*x2      + x1*y2*( i)
       + y1*x2*(i ) + y1*y2*(ii)

> real number
ii = 1 = cos(0)

> complex number
ii = -1 = cos(90 + 90)
*/

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

/*
3D, 4D
left-handed coordinate system

> basis vectors multiplication
ij = -ji = k
jk = -kj = i
ki = -ik = j

> generic vector multiplication
result = (x1*i + y1*j + z1*k + w1) * (x2*i + y2*j + z2*k + w2)
result = x1*i * (x2*i + y2*j + z2*k + w2) = x1*x2*(ii) + x1*y2*(ij) + x1*z2*(ik) + x1*w2*(i )
       + y1*j * (x2*i + y2*j + z2*k + w2) = y1*x2*(ji) + y1*y2*(jj) + y1*z2*(jk) + y1*w2*(j )
       + z1*k * (x2*i + y2*j + z2*k + w2) = z1*x2*(ki) + z1*y2*(kj) + z1*z2*(kk) + z1*w2*(k )
       + w1   * (x2*i + y2*j + z2*k + w2) = w1*x2*( i) + w1*y2*( j) + w1*z2*( k) + w1*w2*(  )

> cross product
ii = jj = kk = ijk =  0 == sin(0)

> dot product
ii = jj = kk = ijk =  1 == cos(0)

> quaternion
ii = jj = kk = ijk = -1 == cos(90 + 90)
*/

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

// complex number

/*
> code assumes unit-length complex numbers for rotation pruposes

> representations
x + y*i == real + imaginary
e^(i * angle) == cos(angle) + i*sin(angle)

*/

cplx cplx_set_radians(r32 radians) { return (cplx){cosf(radians), sinf(radians)}; }

// if `с` is normalized, then `cplx_reciprocal` is equivalent to `cplx_conjugate`
cplx cplx_conjugate(cplx c) { return (cplx){c.x, -c.y}; }
cplx cplx_reciprocal(cplx c) {
	r32 ms = vec2_dot(c, c);
	return vec2_div(cplx_conjugate(c), VEC2_SINGLE(ms));
}

cplx cplx_mul(cplx c1, cplx c2) {
	return (cplx){
		c1.x*c2.x - c1.y*c2.y,
		c1.x*c2.y + c1.y*c2.x,
	};
}

r32 cplx_get_radians(cplx c) { return atan2f(c.y, c.x); }

// vec3, cross product
vec3 vec3_cross(vec3 v1, vec3 v2) {
	return (vec3){
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x,
	};
}

// vec4, quaternion

/*
> code assumes unit-length quaternions for rotation purposes

> representations
x*i + y*j + z*k + w == vector + scalar
e^(axis * angle) == axis*sin(angle) + cos(angle)

make notice, it's `angle`, not `angle/2`; more on that later...

have: arbitrary vector `V`
want: rotate it around an `axis` by an `angle`

> Rodrigues' rotation formula
  V              = V_parallel_to_axis + V_perpendicular_to_axis
  V_para         = axis * dot(V, axis)
  V_para_rotated = V_parallel_to_axis
  V_perp_rotated = V_perp * cos(angle) + (axis x V_perp) * sin(angle)
  V_rotated      = V_para_rotated + V_perp_rotated

- expand the previous formulas
  V_rotated = V_para + V_perp * cos(angle) + (axis x V_perp) * sin(angle)
            = V_para + (V - V_para) * cos(angle) + (axis x (V - V_para)) * sin(angle)
            = V_para * (1 - cos(angle)) + V * cos(angle) + (axis x V) * sin(angle)
            = axis * dot(V, axis) * (1 - cos(angle)) + V * cos(angle) + (axis x V) * sin(angle)

> reasonning quaternion application for rotations
  V_perp_rotated = (axis x V_perp) * sin(angle) + V_perp * cos(angle)
                 = (axis*sin(angle) + cos(angle)) * V_perp
                 = e^(axis * angle) * V_perp

- remember special cases
                e^(axis * angle) * V_para = V_para * e^(axis * angle)
  (axis*sin(angle) + cos(angle)) * V_para = V_para * (axis*sin(angle) + cos(angle))
                      cos(angle) * V_para = V_para * cos(angle)

                         e^(axis * angle) * V_perp = V_perp * e^(-axis * angle)
           (axis*sin(angle) + cos(angle)) * V_perp = V_perp * (axis*sin(angle) + cos(angle))
  (axis x V_perp)*sin(angle) + V_perp * cos(angle) = -(V_perp x axis) * sin(angle) + V_perp * cos(angle)

- shrink V_para and V_perp into a single vector
  V_rotated = V_para_rotated + V_perp_rotated
            = V_para + e^(axis * angle) * V_perp
            = e^(axis * angle/2) * e^(-axis * angle/2) * V_para + e^(axis * angle/2) * e^(axis * angle/2) * V_perp
            = e^(axis * angle/2) * V_para * e^(-axis * angle/2) + e^(axis * angle/2) * V_perp * e^(axis * angle/2)
            = e^(axis * angle/2) * (V_para + V_perp) * e^(-axis * angle/2)
            = e^(axis * angle/2) * V * e^(-axis * angle/2)

... that's why we use half-angle quaternions: to rotate arbitrary vectors without the need to
    split them into parallel and perpendicular parts; at least, that's a partial explanation
*/

quat quat_set_axis(vec3 axis, r32 radians) {
	// construct a half-angle quaternion, for `quat_transform` expects one
	r32 const r = radians * 0.5f;
	r32 const s = sinf(r);
	r32 const c = cosf(r);
	return (quat){axis.x * s, axis.y * s, axis.z * s, c};
}

quat quat_set_radians(vec3 radians) {
	// > result = QUAT(0,0,0,1)
	//          * quat_set_axis(VEC3(0,1,0), radians.y)
	//          * quat_set_axis(VEC3(1,0,0), radians.x)
	//          * quat_set_axis(VEC3(0,0,1), radians.z)
	vec3 const r = vec3_mul(radians, VEC3_SINGLE(0.5f));
	vec3 const s = (vec3){sinf(r.x), sinf(r.y), sinf(r.z)};
	vec3 const c = (vec3){cosf(r.x), cosf(r.y), cosf(r.z)};
	r32 const sy_cx = s.y*c.x; r32 const cy_sx = c.y*s.x;
	r32 const cy_cx = c.y*c.x; r32 const sy_sx = s.y*s.x;
	return (quat){
		sy_cx*s.z + cy_sx*c.z,
		sy_cx*c.z - cy_sx*s.z,
		cy_cx*s.z - sy_sx*c.z,
		cy_cx*c.z + sy_sx*s.z,
	};
}


// if `q` is normalized, then `quat_reciprocal` is equivalent to `quat_conjugate`
quat quat_conjugate(quat q) { return (quat){-q.x, -q.y, -q.z, q.w}; }
quat quat_reciprocal(quat q) {
	r32 ms = vec4_dot(q, q);
	return vec4_div(quat_conjugate(q), VEC4_SINGLE(ms));
}

quat quat_mul(quat q1, quat q2) {
	// result = (v1 + s1) * (v2 + s2)
	//        = v1*v2 + v1*s2 + v2*s1 + s1*s1
	//        = vec3_cross(v1,v2) - vec3_dot(v1,v2) + v1*s2 + v2*s1 + s1*s2
	return (quat){
		 q1.x*q2.w + q1.y*q2.z - q1.z*q2.y + q1.w*q2.x,
		-q1.x*q2.z + q1.y*q2.w + q1.z*q2.x + q1.w*q2.y,
		 q1.x*q2.y - q1.y*q2.x + q1.z*q2.w + q1.w*q2.z,
		-q1.x*q2.x - q1.y*q2.y - q1.z*q2.z + q1.w*q2.w,
	};
}

vec3 quat_transform(quat q, vec3 v) {
	// > result = q * QUAT(v.x, v.y, v.z, 0) * quat_reciprocal(q)
	r32 const xx = q.x*q.x; r32 const yy = q.y*q.y; r32 const zz = q.z*q.z; r32 const ww = q.w*q.w;
	r32 const xy = q.x*q.y; r32 const yz = q.y*q.z; r32 const zw = q.z*q.w; r32 const wx = q.w*q.x;
	r32 const xz = q.x*q.z; r32 const yw = q.y*q.w;
	return (vec3){
		v.x * ( xx - yy - zz + ww) + (v.y * (xy - zw) + v.z * (yw + xz)) * 2,
		v.y * (-xx + yy - zz + ww) + (v.z * (yz - wx) + v.x * (zw + xy)) * 2,
		v.z * (-xx - yy + ww + zz) + (v.x * (xz - yw) + v.y * (yz + wx)) * 2,
	};
}

void quat_get_axes(quat q, vec3 * x, vec3 * y, vec3 * z) {
	// > *x = quat_transform(q, VEC3(1,0,0))
	// > *y = quat_transform(q, VEC3(0,1,0))
	// > *z = quat_transform(q, VEC3(0,0,1))
	r32 const xx = q.x*q.x; r32 const yy = q.y*q.y; r32 const zz = q.z*q.z; r32 const ww = q.w*q.w;
	r32 const xy = q.x*q.y; r32 const yz = q.y*q.z; r32 const zw = q.z*q.w; r32 const wx = q.w*q.x;
	r32 const xz = q.x*q.z; r32 const yw = q.y*q.w;
	*x = (vec3){(xx - yy - zz + ww), (zw + xy) * 2,        (xz - yw) * 2};
	*y = (vec3){(xy - zw) * 2,       (-xx + yy - zz + ww), (yz + wx) * 2};
	*z = (vec3){(yw + xz) * 2,       (yz - wx) * 2,        (-xx - yy + ww + zz)};
}

// mat4

/*
TRANSFORMATION

- XYZ: orientation * scale
- W: offset
- `.w` indicates vector kind: 0 for direction, 1 for position
*/

mat4 mat4_set_transformation(vec3 position, vec3 scale, quat rotation) {
	vec3 axis_x, axis_y, axis_z;
	quat_get_axes(rotation, &axis_x, &axis_y, &axis_z);
	axis_x = vec3_mul(axis_x, VEC3_SINGLE(scale.x));
	axis_y = vec3_mul(axis_y, VEC3_SINGLE(scale.y));
	axis_z = vec3_mul(axis_z, VEC3_SINGLE(scale.z));
	return (mat4){
		{axis_x.x,   axis_x.y,   axis_x.z,   0},
		{axis_y.x,   axis_y.y,   axis_y.z,   0},
		{axis_z.x,   axis_z.y,   axis_z.z,   0},
		{position.x, position.y, position.z, 1},
	};
}

/*
PROJECTION

> parameters
- XYZ: world space vector
- ncp: world space near clipping plane
- fcp: world space far clipping plane
- XYZ': normalized space vector
- NNCP: normalized space near clipping plane
- NFCP: normalized space far clipping plane
- ortho: [0 .. 1], where 0 is perspective mode, 1 is orthographic mode

> orthograhic
- XYZ' = (offset + scale * XYZ) / 1
- XY scale: from [-scale_xy .. scale_xy] to [-1 .. 1]
- Z  scale: from [ncp .. fcp]            to [NNCP .. 1]

> perspective
- XYZ' = (offset + scale * XYZ) / Z
- XY scale; from [-scale_xy .. scale_xy] to [-1 .. 1]
- Z  scale; from [ncp .. fcp]            to [NNCP .. 1]
*/

mat4 mat4_set_projection(vec2 scale_xy, r32 ncp, r32 fcp, r32 ortho) {
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
		{scale_xy.x, 0,          0,        0},
		{0,          scale_xy.y, 0,        0},
		{0,          0,          scale_z,  zw},
		{0,          0,          offset_z, ww},
	};
}

mat4 mat4_inverse_transformation(mat4 m) {
	vec3 position = (vec3){m.w.x, m.w.y, m.w.z};
	return (mat4){
		{m.x.x, m.y.x, m.z.x, 0},
		{m.x.y, m.y.y, m.z.y, 0},
		{m.x.z, m.y.z, m.z.z, 0},
		{
			-vec3_dot(position, VEC3(m.x.x, m.x.y, m.x.z)),
			-vec3_dot(position, VEC3(m.y.x, m.y.y, m.y.z)),
			-vec3_dot(position, VEC3(m.z.x, m.z.y, m.z.z)),
			1
		},
	};
}

vec4 mat4_mul_vec(mat4 m, vec4 v) {
	return (vec4){
		vec4_dot(VEC4(m.x.x, m.y.x, m.z.x, m.w.x), v),
		vec4_dot(VEC4(m.x.y, m.y.y, m.z.y, m.w.y), v),
		vec4_dot(VEC4(m.x.z, m.y.z, m.z.z, m.w.z), v),
		vec4_dot(VEC4(m.x.w, m.y.w, m.z.w, m.w.w), v),
	};
}

mat4 mat4_mul_mat(mat4 m1, mat4 m2) {
	return (mat4){
		mat4_mul_vec(m1, m2.x),
		mat4_mul_vec(m1, m2.y),
		mat4_mul_vec(m1, m2.z),
		mat4_mul_vec(m1, m2.w),
	};
}

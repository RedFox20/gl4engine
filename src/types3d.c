#include "types3d.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define retvec2(x,y)     vec2 _r = {x,y};     return _r
#define retvec3(x,y,z)   vec3 _r = {x,y,z};   return _r
#define retvec4(x,y,z,w) vec4 _r = {x,y,z,w}; return _r

////////////////////////////////////////////////////////////////////////////////

float radf(float degrees) {
	return (degrees * (float)M_PI) / 180.0f; // rads=(degs*PI)/180
}

////////////////////////////////////////////////////////////////////////////////

float vec2_len(vec2 v)   { return sqrtf(v.x*v.x + v.y*v.y); }
float vec2_sqlen(vec2 v) { return v.x*v.x + v.y*v.y; }
vec2 vec2_norm(vec2 v) {
	float sqr = v.x*v.x + v.y*v.y;
	float inv = 1.0f / sqrtf(sqr);
	retvec2(v.x*inv, v.y*inv);
}

////////////////////////////////////////////////////////////////////////////////

vec3 vec3_new(float x, float y, float z) { retvec3(x,y,z); }
vec3 vec3_cross(vec3 a, vec3 b) {
	retvec3(a.y*b.z - b.y*a.z, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y);
}
float vec3_dot(vec3 a, vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
float vec3_len(vec3 v)   { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
float vec3_sqlen(vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
vec3 vec3_norm(vec3 v) {
	float sqr = v.x*v.x + v.y*v.y + v.z*v.z;
	float inv = 1.0f / sqrtf(sqr);
	retvec3(v.x*inv, v.y*inv, v.z*inv);
}
vec3 vec3_add(vec3 a, vec3 b)   { retvec3(a.x+b.x, a.y+b.y, a.z+b.z); }
vec3 vec3_sub(vec3 a, vec3 b)   { retvec3(a.x-b.x, a.y-b.y, a.z-b.z); }
vec3 vec3_mul(vec3 a, vec3 b)   { retvec3(a.x*b.x, a.y*b.y, a.z*b.z); }
vec3 vec3_div(vec3 a, vec3 b)   { retvec3(a.x/b.x, a.y/b.y, a.z/b.z); }
vec3 vec3_addf(vec3 a, float v) { retvec3(a.x+v, a.y+v, a.z+v); }
vec3 vec3_subf(vec3 a, float v) { retvec3(a.x-v, a.y-v, a.z-v); }
vec3 vec3_mulf(vec3 a, float v) { retvec3(a.x*v, a.y*v, a.z*v); }
vec3 vec3_divf(vec3 a, float v) { retvec3(a.x/v, a.y/v, a.z/v); }

////////////////////////////////////////////////////////////////////////////////

vec4 vec4_new(float x, float y, float z, float w) { retvec4(x,y,z,w); }
vec4 vec4_add(vec4 a, vec4 b)   { retvec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
vec4 vec4_sub(vec4 a, vec4 b)   { retvec4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
vec4 vec4_mul(vec4 a, vec4 b)   { retvec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
vec4 vec4_div(vec4 a, vec4 b)   { retvec4(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
vec4 vec4_addf(vec4 a, float v) { retvec4(a.x+v, a.y+v, a.z+v, a.w+v); }
vec4 vec4_subf(vec4 a, float v) { retvec4(a.x-v, a.y-v, a.z-v, a.w-v); }
vec4 vec4_mulf(vec4 a, float v) { retvec4(a.x*v, a.y*v, a.z*v, a.w*v); }
vec4 vec4_divf(vec4 a, float v) { retvec4(a.x/v, a.y/v, a.z/v, a.w/v); }

////////////////////////////////////////////////////////////////////////////////

vec4 quat_angle_axis(float angle, vec3 axis)
{
	const float r = radf(angle) * 0.5f;
	const float s = sinf(r);
	retvec4(axis.x * s, axis.y * s, axis.z * s, cosf(r));
}

vec4 quat_angle_axisf(float angle, float x, float y, float z)
{
	vec3 axis = {x,y,z};
	return quat_angle_axis(angle, axis);
}

vec4 quat_from_rotation(vec3 rotation)
{
	vec4 q = quat_angle_axisf(rotation.x, 1.0f, 0.0f, 0.0f);
	     q = quat_mul(quat_angle_axisf(rotation.y, 0.0f, 1.0f, 0.0f), q);
	return   quat_mul(quat_angle_axisf(rotation.z, 0.0f, 0.0f, 1.0f), q);
}

vec4 quat_mul(vec4 q, vec4 p)
{
	retvec4(
		q.w*p.w - q.x*p.x - q.y*p.y - q.z*p.z,
		q.w*p.x + q.x*p.w + q.y*p.z - q.z*p.y,
		q.w*p.y + q.y*p.w + q.z*p.x - q.x*p.z,
		q.w*p.z + q.z*p.w + q.x*p.y - q.y*p.x
	);
}

////////////////////////////////////////////////////////////////////////////////

const mat4 IDENTITY = {{{
	1, 0, 0, 0, 
	0, 1, 0, 0, 
	0, 0, 1, 0, 
	0, 0, 0, 1,
}}};

mat4* mat4_identity(mat4* m)
{
	*m = IDENTITY;
	return m;
}

mat4* mat4_mul(mat4* ma, const mat4* mb)
{
	const vec4 a0 = ma->r0;
	const vec4 a1 = ma->r1;
	const vec4 a2 = ma->r2;
	const vec4 a3 = ma->r3;
	const vec4 b0 = mb->r0;
	const vec4 b1 = mb->r1;
	const vec4 b2 = mb->r2;
	const vec4 b3 = mb->r3;
	ma->r0 = vec4_add( vec4_add(vec4_mulf(a0,b0.x), vec4_mulf(a1,b0.y)),  vec4_add(vec4_mulf(a2,b0.z), vec4_mulf(a3,b0.w)) );
	ma->r1 = vec4_add( vec4_add(vec4_mulf(a0,b1.x), vec4_mulf(a1,b1.y)),  vec4_add(vec4_mulf(a2,b1.z), vec4_mulf(a3,b1.w)) );
	ma->r2 = vec4_add( vec4_add(vec4_mulf(a0,b2.x), vec4_mulf(a1,b2.y)),  vec4_add(vec4_mulf(a2,b2.z), vec4_mulf(a3,b2.w)) );
	ma->r3 = vec4_add( vec4_add(vec4_mulf(a0,b3.x), vec4_mulf(a1,b3.y)),  vec4_add(vec4_mulf(a2,b3.z), vec4_mulf(a3,b3.w)) );
	return ma;
	//float x = ma->m00, y = ma->m01, z = ma->m02, w = ma->m03;
	//ma->m00 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	//ma->m01 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	//ma->m02 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	//ma->m03 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	//x = ma->m10, y = ma->m11, z = ma->m12, w = ma->m13;
	//ma->m10 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	//ma->m11 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	//ma->m12 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	//ma->m13 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	//x = ma->m20, y = ma->m21, z = ma->m22, w = ma->m23;
	//ma->m20 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	//ma->m21 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	//ma->m22 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	//ma->m23 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	//x = ma->m30, y = ma->m31, z = ma->m32, w = ma->m33;
	//ma->m30 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	//ma->m31 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	//ma->m32 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	//ma->m33 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	//return ma;
}

vec4 mat4_mul3(mat4* m, vec3 v)
{
	retvec4(
		(m->m00*v.x) + (m->m10*v.y) + (m->m20*v.z) + m->m30,
		(m->m01*v.x) + (m->m11*v.y) + (m->m21*v.z) + m->m31,
		(m->m02*v.x) + (m->m12*v.y) + (m->m22*v.z) + m->m32,
		(m->m03*v.x) + (m->m13*v.y) + (m->m23*v.z) + m->m33
	);
}
vec4 mat4_mul4(mat4* m, vec4 v)
{
	retvec4(
		(m->m00*v.x) + (m->m10*v.y) + (m->m20*v.z) + (m->m30*v.w),
		(m->m01*v.x) + (m->m11*v.y) + (m->m21*v.z) + (m->m31*v.w),
		(m->m02*v.x) + (m->m12*v.y) + (m->m22*v.z) + (m->m32*v.w),
		(m->m03*v.x) + (m->m13*v.y) + (m->m23*v.z) + (m->m33*v.w)
	);
}

mat4* mat4_translate(mat4* m, vec3 v)
{
	m->r3 = mat4_mul3(m, v);
	return m;
}

static vec4 _mat4_rotrow(mat4* m, vec3 rot)
{
	return vec4_add(vec4_add(
				vec4_mulf(m->r0, rot.x),
				vec4_mulf(m->r1, rot.y)),
				vec4_mulf(m->r2, rot.z));
}
mat4* mat4_rotate(mat4* m, float angleDegs, vec3 rotationAxis)
{
	float rad = radf(angleDegs);
	float c = cosf(rad);
	vec3 axis  = vec3_norm(rotationAxis);
	vec3 temp  = vec3_mulf(axis, 1.0f - c);
	vec3 sAxis = vec3_mulf(axis, sinf(rad));
	vec3 r0 = { c + temp.x*axis.x, 
		            temp.x*axis.y + sAxis.z,
		            temp.x*axis.z - sAxis.y, };
	vec3 r1 = {     temp.y*axis.x - sAxis.z,
			    c + temp.y*axis.y, 
		            temp.y*axis.z + sAxis.x, };
	vec3 r2 = {     temp.z*axis.x + sAxis.y,
				    temp.z*axis.y - sAxis.x, 
		        c + temp.z*axis.z };
	m->r0 = _mat4_rotrow(m, r0);
	m->r1 = _mat4_rotrow(m, r1);
	m->r2 = _mat4_rotrow(m, r2);
	return m;
}
mat4* mat4_rotatef(mat4* m, float angleDegs, float x, float y, float z)
{
	vec3 axis = {x,y,z};
	return mat4_rotate(m, angleDegs, axis);
}

mat4* mat4_scale(mat4* m, vec3 scale)
{
	m->m00 *= scale.x;
	m->m11 *= scale.y;
	m->m22 *= scale.z;
	return m;
}

mat4 mat4_ortho(float left, float right, float bottom, float top)
{
	const float rl = right - left;
	const float tb = top - bottom;
	const mat4 r = {{{
		2.0f / rl, 0, 0, 0,
		0, 2.0f / tb, 0, 0,
		0, 0, -1.0f, 0,
		-(right+left)/rl, -(top+bottom)/tb, 0, 1,
	}}};
	return r;
}

mat4 mat4_perspective(float fov, float width, float height, float zNear, float zFar)
{
	const float rad2 = radf(fov) * 0.5f;
	const float h = cosf(rad2) / sinf(rad2);
	const float w = (h * height) / width;
	const float range = zFar - zNear;
	const mat4 r = {{{
		w, 0, 0, 0, 
		0, h, 0, 0, 
		0, 0, -(zFar + zNear) / range, -1, 
		0, 0, (-2.0f * zFar * zNear) / range, 1,
	}}};
	return r;
}

mat4 mat4_lookat(vec3 eye, vec3 center, vec3 up) // create a lookat mat4
{
	const vec3 f = vec3_norm(vec3_sub(center, eye));
	const vec3 s = vec3_norm(vec3_cross(f, vec3_norm(up)));
	const vec3 u = vec3_cross(s, f);
	const mat4 r = {{{
		s.x, u.x, -f.x, 0.0f,
		s.y, u.y, -f.y, 0.0f,
		s.z, u.z, -f.z, 0.0f,
		-vec3_dot(s,eye), -vec3_dot(u,eye), vec3_dot(f,eye), 1.0f,
	}}};
	return r;
}

mat4 mat4_from_rotation(vec3 rotation)
{
	vec4 q = quat_from_rotation(rotation);
	mat4 m = IDENTITY;
	m.m00 = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
	m.m01 = 2 * q.x * q.y + 2 * q.w * q.z;
	m.m02 = 2 * q.x * q.z - 2 * q.w * q.y;
	m.m10 = 2 * q.x * q.y - 2 * q.w * q.z;
	m.m11 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
	m.m12 = 2 * q.y * q.z + 2 * q.w * q.x;
	m.m20 = 2 * q.x * q.z + 2 * q.w * q.y;
	m.m21 = 2 * q.y * q.z - 2 * q.w * q.x;
	m.m22 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
	return m;
}

mat4 mat4_from_scale(vec3 scale)
{
	mat4 m = IDENTITY;
	m.m00 *= scale.x;
	m.m11 *= scale.y;
	m.m22 *= scale.z;
	return m;
}

#include "types3d.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define retvec2(x,y)     vec2 r = {x,y};     return r
#define retvec3(x,y,z)   vec3 r = {x,y,z};   return r
#define retvec4(x,y,z,w) vec4 r = {x,y,z,w}; return r

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
	float x = ma->m00, y = ma->m01, z = ma->m02, w = ma->m03;
	ma->m00 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	ma->m01 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	ma->m02 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	ma->m03 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	x = ma->m10, y = ma->m11, z = ma->m12, w = ma->m13;
	ma->m10 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	ma->m11 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	ma->m12 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	ma->m13 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	x = ma->m20, y = ma->m21, z = ma->m22, w = ma->m23;
	ma->m20 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	ma->m21 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	ma->m22 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	ma->m23 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	x = ma->m30, y = ma->m31, z = ma->m32, w = ma->m33;
	ma->m30 = (mb->m00*x) + (mb->m10*y) + (mb->m20*z) + (mb->m30*w);
	ma->m31 = (mb->m01*x) + (mb->m11*y) + (mb->m21*z) + (mb->m31*w);
	ma->m32 = (mb->m02*x) + (mb->m12*y) + (mb->m22*z) + (mb->m32*w);
	ma->m33 = (mb->m03*x) + (mb->m13*y) + (mb->m23*z) + (mb->m33*w);
	return ma;
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

mat4* mat4_scale(mat4* m, vec3 scale)
{
	m->r0 = vec4_mulf(m->r0, scale.x);
	m->r1 = vec4_mulf(m->r1, scale.y);
	m->r2 = vec4_mulf(m->r2, scale.z);
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
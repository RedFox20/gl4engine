#pragma once

////////////////////////////////////////////////////////////////////////////////

float radf(float degrees);

////////////////////////////////////////////////////////////////////////////////

typedef unsigned int index_t; // vertex index type 

typedef struct vec2
{
	float x, y;
} vec2; // 2D position vector - typical UV container or 2D coordinate

typedef struct vec3
{
	float x, y, z;
} vec3; // 3D position vector - typical XYZ container

typedef struct vec4
{
	float x, y, z, w;
} vec4; // 4D float vector - used for Quaternions and RGBA colors


vec3 vec3_cross(vec3 a, vec3 b); // vec3 cross product:  r = a x b
float vec3_dot(vec3 a, vec3 b);  // vec3 dot product:    r = a * b
vec3 vec3_norm(vec3 v);          // vec3 normalize:  
vec3 vec3_add(vec3 a, vec3 b);   // vec3 add:  r = a + b
vec3 vec3_sub(vec3 a, vec3 b);   // vec3 sub:  r = a - b
vec3 vec3_mul(vec3 a, vec3 b);   // vec3 mul:  r = a * b
vec3 vec3_div(vec3 a, vec3 b);   // vec3 div:  r = a / b
vec3 vec3_addf(vec3 a, float v);
vec3 vec3_subf(vec3 a, float v);
vec3 vec3_mulf(vec3 a, float v);
vec3 vec3_divf(vec3 a, float v);

vec4 vec4_add(vec4 a, vec4 b);   // vec4 add:  r = a + b
vec4 vec4_sub(vec4 a, vec4 b);   // vec4 sub:  r = a - b
vec4 vec4_mul(vec4 a, vec4 b);   // vec4 mul:  r = a * b
vec4 vec4_div(vec4 a, vec4 b);   // vec4 div:  r = a / b
vec4 vec4_addf(vec4 a, float v);
vec4 vec4_subf(vec4 a, float v);
vec4 vec4_mulf(vec4 a, float v);
vec4 vec4_divf(vec4 a, float v);


////////////////////////////////////////////////////////////////////////////////

typedef struct vertex_t
{
	union {
		struct { float x, y, z; }; // 3d XYZ coordinates
		vec3 pos;                  // alias for XYZ
	};
	union {
		struct { float u, v; };    // UV coordinates
		vec2 tex;                  // alias for UV
	};
	union {
		struct { float nx, ny, nw; }; // 3d XYZ normal direction
		vec3 norm;                    // alias for normal
	};
} vertex_t; // 3D vertex consisting of POS3D,TEX2D,NORM3D

////////////////////////////////////////////////////////////////////////////////
// A 4x4 matrix for affine transformations
typedef struct mat4
{
	union {
		struct {
			float m00, m01, m02, m03; // row0  0-3
			float m10, m11, m12, m13; // row1  0-3
			float m20, m21, m22, m23; // row2  0-3
			float m30, m31, m32, m33; // row3  0-3
		};
		float m[16]; // 4x4 float matrix
		struct {
			vec4 r0, r1, r2, r3; // rows 0-3
		};
		vec4 r[4]; // rows 0-3
	};
} mat4;

// global identity matrix for easy initialization
extern const mat4 IDENTITY;

// initializes a new identity matrix
mat4* mat4_identity(mat4* m);

// multiplies two matrices:  ma = ma * mb
mat4* mat4_mul(mat4* ma, const mat4* mb);

// transforms 3D vector v with this matrix and return a vec4
vec4 mat4_mul3(mat4* m, vec3 v);

// translates object transformation matrix by given offset
mat4* mat4_translate(mat4* m, vec3 offset);

// rotates an object transformation matrix by given degree angle around a given axis
mat4* mat4_rotate(mat4* m, float angleDegs, vec3 axis);

// scales an object transformation matrix by a given scale factor
mat4* mat4_scale(mat4* m, vec3 scale);

// creates an ortographic projection matrix
mat4 mat4_ortho(float left, float right, float bottom, float top);

// creates a perspective projection matrix
mat4 mat4_perspective(float fov, float width, float height, float zNear, float zFar);

// creates a lookat view/camera matrix
mat4 mat4_lookat(vec3 eye, vec3 center, vec3 up);

////////////////////////////////////////////////////////////////////////////////
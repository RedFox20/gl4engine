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

extern const vec2 vec2_ZERO; // Represents vec2 {0,0}
extern const vec3 vec3_ZERO; // Represents vec3 {0,0,0}
extern const vec4 vec4_ZERO; // Represents vec4 {0,0,0,0}
extern const vec3 vec3_XAXIS; // Represents vec3 {1,0,0}
extern const vec3 vec3_YAXIS; // Represents vec3 {0,1,0}
extern const vec3 vec3_ZAXIS; // Represents vec3 {0,0,1}

////////////////////////////////////////////////////////////////////////////////

float vec2_len(vec2 v);
float vec2_sqlen(vec2 v);
vec2 vec2_norm(vec2 v);

////////////////////////////////////////////////////////////////////////////////

vec3 vec3_new(float x, float y, float z);
vec3 vec3_cross(vec3 a, vec3 b); // vec3 cross product:  r = a x b
float vec3_dot(vec3 a, vec3 b);  // vec3 dot product:    r = a * b
float vec3_len(vec3 v);
float vec3_sqlen(vec3 v);
vec3 vec3_norm(vec3 v);          // vec3 normalize:  
vec3 vec3_add(vec3 a, vec3 b);   // vec3 add:  r = a + b
vec3 vec3_sub(vec3 a, vec3 b);   // vec3 sub:  r = a - b
vec3 vec3_mul(vec3 a, vec3 b);   // vec3 mul:  r = a * b
vec3 vec3_div(vec3 a, vec3 b);   // vec3 div:  r = a / b
vec3 vec3_addf(vec3 a, float v);
vec3 vec3_subf(vec3 a, float v);
vec3 vec3_mulf(vec3 a, float v);
vec3 vec3_divf(vec3 a, float v);

////////////////////////////////////////////////////////////////////////////////

vec4 vec4_new(float x, float y, float z, float w);
void vec4_set(vec4* v, float x, float y, float z, float w);
vec4 vec4_add(vec4 a, vec4 b);   // vec4 add:  r = a + b
vec4 vec4_sub(vec4 a, vec4 b);   // vec4 sub:  r = a - b
vec4 vec4_mul(vec4 a, vec4 b);   // vec4 mul:  r = a * b
vec4 vec4_div(vec4 a, vec4 b);   // vec4 div:  r = a / b
vec4 vec4_addf(vec4 a, float v);
vec4 vec4_subf(vec4 a, float v);
vec4 vec4_mulf(vec4 a, float v);
vec4 vec4_divf(vec4 a, float v);

////////////////////////////////////////////////////////////////////////////////


// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
vec4 quat_angle_axis(float angle, vec3 axis);

// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
vec4 quat_angle_axisf(float angle, float x, float y, float z);

// creates a quaternion rotation from euler XYZ (degrees) rotation 
vec4 quat_from_rotation(vec3 rotation);

// rotates quaternion p with extra rotation p
// q = additional rotation we wish to rotate with
// p = original rotation
vec4 quat_mul(vec4 q, vec4 p);

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

typedef struct _mat4_row_vis
{
	float x,y,z,w;
} _mat4_row_vis;

typedef struct mat4
{
	union {
		struct {
			_mat4_row_vis vis0, vis1, vis2, vis3;
		};
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

// transforms 3D vector v with this matrix and return the resulting vec4
vec4 mat4_mul3(mat4* m, vec3 v);

// transforms 4D vector v with this matrix and returns the resulting vec4
vec4 mat4_mul4(mat4* m, vec4 v);

// translates object transformation matrix by given offset
mat4* mat4_translate(mat4* m, vec3 offset);

// rotates an object transformation matrix by given degree angle around a given axis
mat4* mat4_rotate(mat4* m, float angleDegs, vec3 axis);

// rotates an object transformation matrix by given degree angle around a given axis
mat4* mat4_rotatef(mat4* m, float angleDegs, float x, float y, float z);

// scales an object transformation matrix by a given scale factor
mat4* mat4_scale(mat4* m, vec3 scale);

// creates an ortographic projection matrix
mat4* mat4_ortho(mat4* out, float left, float right, float bottom, float top);

// creates a perspective projection matrix
mat4* mat4_perspective(mat4* out, float fov, float width, float height, float zNear, float zFar);

// creates a lookat view/camera matrix
mat4* mat4_lookat(mat4* out, vec3 eye, vec3 center, vec3 up);

// creates a translated matrix from XYZ position
mat4* mat4_from_position(mat4* out, vec3 position);

// creates a rotated matrix from euler XYZ rotation
mat4* mat4_from_rotation(mat4* out, vec3 rotation);

// creates a scaled matrix from XYZ scale
mat4* mat4_from_scale(mat4* out, vec3 scale);

////////////////////////////////////////////////////////////////////////////////
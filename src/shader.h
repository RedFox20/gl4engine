#pragma once
#include <stdbool.h>
#include <sys/stat.h> // stat, fstat, time_t
#include "types3d.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

/** @brief shader uniform slots */
typedef enum ShaderUniform
{
	u_Transform,    // uniform matrix transform;      model-view-project matrix
	u_DiffuseTex,   // uniform sampler2D diffuseTex;  diffuse texture
	u_SpecularTex,  // uniform sampler2D specularTex; specular texture
	u_NormalTex,    // uniform sampler2D normalTex;   normal texture
	u_ShadowTex,    // uniform sampler2D shadowTex;   shadow texture
	u_OccludeTex,   // uniform sampler2D occludeTex;  occlusion texture for fake SSAO
	u_DiffuseColor, // uniform vec4 diffuseColor;     diffuse color 
	u_OutlineColor, // uniform vec4 outlineColor;     background or outline color
	u_MaxUniforms,  // uniform counter
} ShaderUniform;


/** @brief shader attribute slots */
typedef enum ShaderAttr
{
	a_Position,      // attribute vec3 position;  position (vec2 XY or vec3 XYZ)
	a_Normal,        // attribute vec3 normal;    normal 
	a_Coord,         // attribute vec2 coord;     texture coordinate 0
	a_Coord2,        // attribute vec2 coord2;    texture coordinate 1
	a_Vertex,        // attribute vec4 vertex;    additional generic 4D vertex
	a_Color,         // attribute vec4 color;     per-vertex coloring
	a_MaxAttributes, // attribute counter
} ShaderAttr;

////////////////////////////////////////////////////////////////////////////////

typedef struct Shader
{
	Resource res;

	unsigned int program; // STRONG_REF: linked glProgram

	char vs_path[120]; // vert shader path
	char fs_path[120]; // frag shader path

	time_t vs_mod; // last modified time of vert shader file
	time_t fs_mod; // last modified time of frag shader file

	char uniforms[u_MaxUniforms];     // uniform locations
	bool attributes[a_MaxAttributes]; // attribute present? true/false

} Shader;

typedef struct ShaderManager 
{
	ResManager rm;
} ShaderManager;

////////////////////////////////////////////////////////////////////////////////
// Implements ResManager Resource interface

// loads a shader, if it's already loaded, simply increments refcount
Shader* shader_load(ShaderManager* m, const char* shaderName);
// decrements refcount, but does not free any resources! use meshmgr_clean_unused()
void shader_free(Shader* s);
// initializes a resource manager for ShaderManager Shader objects
ShaderManager* shader_manager_create(int maxCount);
// destroys the resource manager and all its items
void shader_manager_destroy(ShaderManager* m);
// returns pointer to the first resource element
Shader* shader_manager_data(ShaderManager* m);
// frees any unused (refcount == 0) resources
void shader_manager_clean_unused(ShaderManager* m);
// destroys all items, regardless of their refcounts
void shader_manager_destroy_all_items(ShaderManager* m);

////////////////////////////////////////////////////////////////////////////////

/** 
 * Forces the shader to reload itself. This can also be called after shader_init 
 * @note If reload fails, the original shader program is kept unmodified (!)
 * @return TRUE if shader reload succeeded.
 */
bool shader_reload(Shader* s);

/**
 * Checks vertex/fragment shader sources and does a shader_reload() if necessary.
 * @return TRUE if a successful shader_reload() was performed.
 * @note If shader_reload() fails, this function will return FALSE
 */
bool shader_hotload(Shader* s);

/** Loads shader uniform locations */
void shader_load_uniforms(Shader* s);

/** Binds this shader as the active shader in the pipeline */
void shader_bind(const Shader* s);

/** Unbinds a shader from the pipeline (for whatever reason) */
void shader_unbind(const Shader* s);

////////////////////////////////////////////////////////////////////////////////

/** @brief Binds a mat4 matrix to the specified uniform slot in the shader */
void shader_bind_mat(const Shader* s, int u_uniformSlot, const mat4* matrix);
/** @brief Binds an OpenGL texture handle to the specified uniform slot in the shader */
void shader_bind_tex(const Shader* s, int u_uniformSlot, int glTex2DSlot, unsigned glTexture);
/** @brief Binds a vec4 to the specified uniform slot in the shader */
void shader_bind_vec2(const Shader* s, int u_uniformSlot, vec2 value);
/** @brief Binds a vec4 to the specified uniform slot in the shader */
void shader_bind_vec3(const Shader* s, int u_uniformSlot, vec3 value);
/** @brief Binds a vec4 to the specified uniform slot in the shader */
void shader_bind_vec4(const Shader* s, int u_uniformSlot, vec4 value);

////////////////////////////////////////////////////////////////////////////////

/** @brief Binds a single model-view-projection transformation matrix to shader u_Transform slot. */
void shader_bind_mat_mvp(const Shader* s, const mat4* viewProjection, const mat4* modelWorldTransform);

/** @brief Binds an OpenGL texture handle to u_DiffuseTex uniform slot */
void shader_bind_tex_diffuse(const Shader* s, unsigned glTexture);

/** @brief Binds an OpenGL texture handle to u_DiffuseTex uniform slot */
void shader_bind_color_diffuse(const Shader* s, vec4 color);

////////////////////////////////////////////////////////////////////////////////

/** @brief Binds all the active attributes in the shader */
void shader_bind_attributes(const Shader* s);

/** @brief Unbinds all the active attributes in the shader (required for state coherency!) */
void shader_unbind_attributes(const Shader* s);

////////////////////////////////////////////////////////////////////////////////
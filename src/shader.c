#include "shader.h"
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>   // alloca
#include <stdarg.h>   // va_begin

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

////////////////////////////////////////////////////////////////////////////////

static const char UniformMap[][16] = {
	"transform",     // u_Transform
	"diffuseTex",    // u_DiffuseTex
	"specularTex",   // u_SpecularTex
	"normalTex",     // u_NormalTex
	"shadowTex",     // u_ShadowTex
	"occludeTex",    // u_OccludeTex
	"diffuseColor",  // u_DiffuseColor
	"outlineColor",  // u_OutlineColor
};
static const char AttributeMap[][8] = {
	"position",      // a_Position
	"normal",        // a_Normal
	"coord",         // a_Coord
	"coord2",        // a_Coord2
	"vertex",        // a_Vertex
	"color",         // a_Color
};

static const char* uniform_name(int uniformSlot) {
	if (0 <= uniformSlot && uniformSlot < u_MaxUniforms)
		return UniformMap[uniformSlot];
	return "u_invalid";
}
//static const char* attribute_name(int attributeSlot) {
//	if (0 <= attributeSlot && attributeSlot < a_MaxAttributes)
//		return AttributeMap[attributeSlot];
//	return "a_invalid";
//}

////////////////////////////////////////////////////////////////////////////////

#if DEBUG
static void checkShaderLog(int program)
{
	int logLength = 0;
	bool prog = glIsProgram(program);
	if (prog) glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	else      glGetShaderiv(program,  GL_INFO_LOG_LENGTH, &logLength);
	
	if (logLength > 0) {
		char* log = alloca(logLength);
		if (prog) glGetProgramInfoLog(program, logLength, &logLength, log);
		else      glGetShaderInfoLog(program,  logLength, &logLength, log);
		fprintf(stderr, "%s\n", log);
	}
}
#else
#define checkShaderLog(x) /*do nothing*/
#endif
static GLuint compileShader(const char* shMem, int size, const char* idstr, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shMem, &size);
	glCompileShader(shader);
	checkShaderLog(shader); // this can be a warning

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		fprintf(stderr, "shader_load(): failed to compile '%s'\n", idstr);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
static GLuint compileShaderFile(const char* shFile, time_t* modified, GLenum type)
{
	FILE* f = fopen(shFile, "rb");
	if (!f) {
		fprintf(stderr, "shader_load(): failed to load file '%s'\n", shFile);
		return 0;
	}

	struct stat s; fstat(fileno(f), &s);
	*modified   = s.st_mtime;
	size_t size = s.st_size;

	char* shMem = (char*)(size <= 65536 ? alloca(size) : malloc(size));
	fread(shMem, size, 1, f);
	fclose(f);

	GLuint shader = compileShader(shMem, size, shFile, type);
	if (size > 65536) free(shMem);
	return shader;
}

////////////////////////////////////////////////////////////////////////////////

Shader* shader_init(Shader* s, const char* shaderName)
{
	s->program = 0;
	snprintf(s->vs_path, sizeof(s->vs_path), "%s.vert", shaderName);
	snprintf(s->fs_path, sizeof(s->fs_path), "%s.frag", shaderName);
	s->vs_mod = 0;
	s->fs_mod = 0;
	memset(s->uniforms,   -1, sizeof(s->uniforms));
	memset(s->attributes, -1, sizeof(s->attributes));
	return s;
}

void shader_free(Shader* s)
{
	if (s->program) glDeleteProgram(s->program);
}

static void shader_err(const Shader* s, const char* errfmt, ...)
{
	va_list ap;
	va_start(ap, errfmt);
	vfprintf(stderr, errfmt, ap);
	fprintf(stderr, " in {%s|%s}\n", s->vs_path, s->fs_path);
}

bool shader_reload(Shader* s)
{
	GLuint vs = compileShaderFile(s->vs_path, &s->vs_mod, GL_VERTEX_SHADER);
	GLuint fs = compileShaderFile(s->fs_path, &s->fs_mod, GL_FRAGMENT_SHADER);

	int status = vs && fs; // ok
	if (status) {
		glDeleteProgram(s->program);
		GLuint program = s->program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);

		// bind our hardcoded attribute locations:
		for (int i = 0; i < a_MaxAttributes; ++i)
			glBindAttribLocation(program, i, AttributeMap[i]);

		glLinkProgram(program);
		glValidateProgram(program);
		checkShaderLog(program); // this can be a warning
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		if (!status) 
			shader_err(s, "shader_load(): program validate failed");
		else 
			shader_load_uniforms(s);
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	return status;
}

bool shader_load(Shader* s, const char* shaderName)
{
	bool ok = shader_reload(shader_init(s, shaderName));
	if (!ok) shader_free(s);
	return ok;
}

static time_t time_modified(const char* file) {
	struct stat st;
	return stat(file, &st) == 0 ? st.st_mtime : 0;
}
bool shader_hotload(Shader* s)
{
	if (time_modified(s->vs_path) != s->vs_mod) return shader_reload(s);
	if (time_modified(s->fs_path) != s->fs_mod) return shader_reload(s);
	return false;
}

void shader_load_uniforms(Shader* s)
{
	// brute force load all supported uniform and attribute locations
	int numActive;
	glGetProgramiv(s->program, GL_ACTIVE_UNIFORMS, &numActive);
	for (int i = 0; numActive && i < u_MaxUniforms; ++i) {
		int loc = glGetUniformLocation(s->program, UniformMap[i]);
		if (loc != -1) --numActive;
		s->uniforms[i] = (char)loc;
	}
}

void shader_bind(const Shader* s)
{
	glUseProgram(s->program);
}
void shader_unbind(const Shader* s)
{
	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

static void check_uniform(const Shader* s, const char* where, int u_uniformSlot) {
	if (s->uniforms[u_uniformSlot] == -1)
		shader_err(s, "%s: uniform '%s' not found", where, uniform_name(u_uniformSlot));
}

void shader_bind_mat(const Shader* s, int u_uniformSlot, const mat4 * matrix)
{
	check_uniform(s, "shader_bind_mat()", u_uniformSlot);
	glUniformMatrix4fv(s->uniforms[u_uniformSlot], 1, GL_FALSE, matrix->m);
}
void shader_bind_tex(const Shader* s, int u_uniformSlot, int glTex2DSlot, unsigned glTexture)
{
	check_uniform(s, "shader_bind_tex()", u_uniformSlot);
	glBindTexture(GL_TEXTURE_2D + glTex2DSlot, glTexture);
	glUniform1i(s->uniforms[u_DiffuseTex], glTex2DSlot); // GL_TEXTURE_2D [glTex2DSlot]
}
void shader_bind_vec2(const Shader* s, int u_uniformSlot, vec2 value)
{
	check_uniform(s, "shader_bind_vec2()", u_uniformSlot);
	glUniform2fv(u_uniformSlot, 1, &value.x);
}
void shader_bind_vec3(const Shader* s, int u_uniformSlot, vec3 value)
{
	check_uniform(s, "shader_bind_vec3()", u_uniformSlot);
	glUniform3fv(u_uniformSlot, 1, &value.x);
}
void shader_bind_vec4(const Shader* s, int u_uniformSlot, vec4 value)
{
	check_uniform(s, "shader_bind_vec4()", u_uniformSlot);
	glUniform4fv(u_uniformSlot, 1, &value.x);
}

////////////////////////////////////////////////////////////////////////////////

void shader_bind_mat_mvp(const Shader* s, const mat4* viewProjection, 
	                                      const mat4* modelWorldTransform)
{
	mat4 mvp = *viewProjection;
	mat4_mul(&mvp, modelWorldTransform); // mvp = viewProjection * transform
	shader_bind_mat(s, u_Transform, &mvp);
}
void shader_bind_tex_diffuse(const Shader* s, unsigned glTexture)
{
	shader_bind_tex(s, u_DiffuseTex, 0, glTexture);
}
void shader_bind_color_diffuse(const Shader* s, vec4 color)
{	
	shader_bind_vec4(s, u_DiffuseColor, color);
}

////////////////////////////////////////////////////////////////////////////////

void shader_bind_attributes(const Shader* s)
{
	for (int i = 0; i < a_MaxAttributes; ++i)
		if (s->attributes[i]) 
			glEnableVertexAttribArray(i);
}
void shader_unbind_attributes(const Shader* s)
{
	for (int i = 0; i < a_MaxAttributes; ++i)
		if (s->attributes[i]) 
			glDisableVertexAttribArray(i);
}

////////////////////////////////////////////////////////////////////////////////


// initializes a resource manager for ShaderManager Shader objects
ShaderManager* shadermgr_init(int maxCount) {
	return (ShaderManager*)resmgr_init(maxCount, sizeof(Shader), 
		(ResMgr_LoadFunc)shader_load, (ResMgr_FreeFunc)shader_free);
}
Shader* shadermgr_data(ShaderManager* m) {
	return (Shader*)resmgr_data(&m->rm);
}
Shader* shadermgr_load_shader(ShaderManager* m, const char* shaderName) {
	return (Shader*)resmgr_load_item(&m->rm, shaderName);
}
void shadermgr_free_shader(Shader* s) {
	resmgr_free_item(&s->res);
}
void shadermgr_clean_unused(ShaderManager* m) {
	resmgr_clean_unused(&m->rm);
}
void shadermgr_destroy_all_items(ShaderManager* m) {
	resmgr_destroy_all_items(&m->rm);
}
void shadermgr_destroy(ShaderManager* m) {
	resmgr_destroy(&m->rm);
}

////////////////////////////////////////////////////////////////////////////////
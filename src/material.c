#include "material.h"
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

////////////////////////////////////////////////////////////////////////////////

static void texmgr_free(Texture* tex)
{
	if (tex->glTexture) glDeleteTextures(1, &tex->glTexture);
	if (tex->data)      free(tex->data);
}
static bool texmgr_load(Texture* tex, const char* fullPath)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fullPath, &width, &height, 0, SOIL_LOAD_RGBA);
	if (!image) {
		LOG("load_image() failed: '%s'\n", fullPath);
		return false;
	}

	tex->data = NULL;
	glGenTextures(1, &tex->glTexture);
	glBindTexture(GL_TEXTURE_2D, tex->glTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

TexManager* texmgr_init(int maxCount) {
	return (TexManager*)resmgr_init(maxCount, sizeof(Texture), 
		(ResMgr_LoadFunc)texmgr_load, (ResMgr_FreeFunc)texmgr_free);
}
Texture* texmgr_data(TexManager* t) {
	return (Texture*)resmgr_data(&t->rm);
}
Texture* texmgr_load_tex(TexManager* t, const char* texturePath) {
	return (Texture*)resmgr_load_item(&t->rm, texturePath);
}
void texmgr_free_tex(Texture* tex) {
	resmgr_free_item(&tex->res);
}
void texmgr_clean_unused(TexManager* t) {
	resmgr_clean_unused(&t->rm);
}
void texmgr_destroy_all_items(TexManager* t) {
	resmgr_destroy_all_items(&t->rm);
}
void texmgr_destroy(TexManager* t) {
	resmgr_destroy(&t->rm);
}
////////////////////////////////////////////////////////////////////////////////

static const vec4 WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };

Material material_create(Shader* shader, Texture* texture)
{
	Material m;
	m.color   = WHITE;
	m.shader  = shader;
	m.texture = texture;
	return m;
}
Material material_from_file(ShaderManager* smgr, const char* shaderName, 
                            TexManager*    tmgr, const char* texturePath)
{
	Material m;
	m.color   = WHITE;
	m.shader  = shadermgr_load_shader(smgr, shaderName);
	m.texture = texmgr_load_tex(tmgr, texturePath);
	return m;
}

void material_destroy(Material* m)
{
	if (m->shader)  shadermgr_free_shader(m->shader), m->shader  = NULL;
	if (m->texture) texmgr_free_tex(m->texture),      m->texture = NULL;
}

void material_move(Material* dst, Material* src)
{
	Shader*  s = dst->shader;
	Texture* t = dst->texture;
	dst->shader  = src->shader;
	dst->texture = src->texture;
	src->shader  = s;
	src->texture = t;
}


////////////////////////////////////////////////////////////////////////////////
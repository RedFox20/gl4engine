#include "material.h"
#include <GL/glew.h>   // glGenTextures etc.
#include <SOIL/SOIL.h> // SOIL_load_image
#include <stdlib.h>    // free
#include "util.h"      // LOG

////////////////////////////////////////////////////////////////////////////////

static void _tex_free(Texture* tex)
{
	if (tex->glTexture) glDeleteTextures(1, &tex->glTexture);
	if (tex->data)      free(tex->data);
}
static bool _tex_load(Texture* tex, const char* fullPath)
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

TexManager* tex_manager_create(int maxCount) {
	static int id = 0;
	char name[32]; snprintf(name, 32, "tex_manager_$%d_[%d]", id++, maxCount);
	return (TexManager*)res_manager_create(name, maxCount, sizeof(Texture), 
		(ResMgr_LoadFunc)_tex_load, (ResMgr_FreeFunc)_tex_free);
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

void material_destroy(Material* m)
{
	if (m->shader)  resource_free(&m->shader->res),  m->shader  = NULL;
	if (m->texture) resource_free(&m->texture->res), m->texture = NULL;
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
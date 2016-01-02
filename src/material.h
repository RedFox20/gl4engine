#pragma once
#include <stdbool.h>
#include "shader.h"

////////////////////////////////////////////////////////////////////////////////

// Managed by TexManager and refcounted
typedef struct Texture 
{
	Resource res;
	unsigned glTexture; // STRONG REF: OpenGL texture handle
	void*    data;      // STRONG REF: loaded image data (NULL by default, opt to retain)
} Texture;

typedef struct TexManager
{
	ResManager rm;
} TexManager;

////////////////////////////////////////////////////////////////////////////////
// Implements ResManager Resource interface

// loads a texture, if it's already loaded, simply increments refcount
Texture* texture_load(TexManager* t, const char* texturePath);
// releases a texture and decrements its refcount
void texture_free(Texture* texture);
// inititalizes generic resource manager as a texture manager
TexManager* tex_manager_create(int maxCount);
// destroys the resource manager and all its items
void tex_manager_destroy(TexManager* t);
// returns pointer to the first resource element
Texture* tex_manager_data(TexManager* t);
// prune unused (refcount == 0) textures
void tex_manager_clean_unused(TexManager* t);
// destroys all items, regardless of their refcounts
void tex_manager_destroy_all_items(TexManager* t);

////////////////////////////////////////////////////////////////////////////////

typedef struct Material
{
	vec4     color;   // default is RGBA(1, 1, 1, 1)
	Shader*  shader;  // STRONG REF: shader associated with this material
	Texture* texture; // STRONG REF: texture reference (does not own this texture!)
} Material;


// creates a new material by taking ownership (!) of the SHADER and TEXTURE
// always call material_destroy (!)
Material material_create(Shader* shader, Texture* texture);

// creates a new material from file by using the given resource managers
// always call material_destroy (!)
Material material_from_file(ShaderManager* smgr, const char* shaderName, TexManager* tmgr, const char* texturePath);

// destroys a material and releases the texture reference
void material_destroy(Material* m);

// moves material from source to destination, swapping the values
void material_move(Material* dst, Material* src);

////////////////////////////////////////////////////////////////////////////////



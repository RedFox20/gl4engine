#pragma once
#include <stdbool.h>
#include "shader.h"

////////////////////////////////////////////////////////////////////////////////

// Managed by ResManager and refcounted
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

// inititalizes generic resource manager as a texture manager
TexManager* tex_manager_create(int maxCount);

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

// destroys a material and releases the texture reference
void material_destroy(Material* m);

// moves material from source to destination, swapping the values
void material_move(Material* dst, Material* src);

////////////////////////////////////////////////////////////////////////////////



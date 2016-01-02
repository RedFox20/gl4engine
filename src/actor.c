#include "actor.h"
#include <string.h>


////////////////////////////////////////////////////////////////////////////////

void actor_init(Actor* a)
{
	memset(a, 0, sizeof(*a));
}
void actor_destroy(Actor* a)
{
	actor_clear_mesh(a);
	actor_clear_material(a);
}
void actor_clear_mesh(Actor * a)
{
	if (a->mesh) mesh_free(a->mesh);
}
void actor_clear_material(Actor* a)
{
	if (a->material.texture) material_destroy(&a->material);
}

bool actor_mesh(Actor* a, StaticMesh* mesh)
{
	actor_clear_mesh(a);
	return (a->mesh = mesh) != NULL;
}

bool actor_material(Actor* a, Material* mat)
{
	actor_clear_material(a);
	if (mat) material_move(&a->material, mat);
	return a->material.shader && a->material.texture;
}

////////////////////////////////////////////////////////////////////////////////

static mat4* affine_transform(Actor* a, mat4* out)
{
	mat4_identity(out);
	return out;
}

void actor_draw(Actor* a, Shader* shader, const mat4* viewProjection)
{
	mat4 model;
	affine_transform(a, &model);
}


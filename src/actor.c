#include "actor.h"
#include <GL/glew.h>
#include <string.h>
#include "util.h"




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
	if (a->mesh) meshmgr_free_mesh(a->mesh);
}
void actor_clear_material(Actor* a)
{
	if (a->material.texture) material_destroy(&a->material);
}

bool actor_load_mesh(Actor* a, MeshManager* mgr, const char* modelPath)
{
	actor_clear_mesh(a);
	a->mesh = meshmgr_load_mesh(mgr, modelPath);
	return a->mesh != NULL;
}

bool actor_load_material(Actor* a, Material* mat)
{
	actor_clear_material(a);
	material_move(&a->material, mat);
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


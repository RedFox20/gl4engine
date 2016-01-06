#include "actor.h"
#include <string.h>
#include "util.h"
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////

Actor* actor_create(const char* name)
{
	Actor* a = malloc(sizeof(*a));
	actor_init(a, name);
	return a;
}
void actor_destroy(Actor* a)
{
	actor_clear(a);
	free(a);
}

void actor_init(Actor* a, const char* name)
{
	memset(a, 0, sizeof(*a));
	strncpy(a->name, name, sizeof(a->name));
	a->scale = vec3_new(1.0f, 1.0f, 1.0f);
}
void actor_clear(Actor* a)
{
	actor_clear_mesh(a);
	actor_clear_material(a);
}
void actor_clear_mesh(Actor * a)
{
	if (a->mesh) resource_free(&a->mesh->res), a->mesh = 0;
}
void actor_clear_material(Actor* a)
{
	if (a->material.texture) 
		material_destroy(&a->material);
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

void actor_affine_matrix(mat4* out, const Actor* a)
{
	mat4_from_position(out, a->pos);
	mat4_scale(out, a->scale);
	mat4 rot;
	mat4_from_rotation(&rot, a->rot);
	mat4_mul(out, &rot);
}

////////////////////////////////////////////////////////////////////////////////

void actor_draw(Actor* a, const mat4* viewProjection)
{
	Shader*  shader  = a->material.shader;
	Texture* texture = a->material.texture;

	if (!shader || !a->mesh || !a->mesh->array) {
		LOG("actor_draw() error: attempted to draw without a shader or mesh\n");
		return;
	}

	shader_bind(shader); // bind, but don't explicitly unbind
	{
		mat4 model;
		actor_affine_matrix(&model, a);
		shader_bind_mat_mvp(shader, viewProjection, &model);
		shader_bind_tex_diffuse(shader, texture->glTexture);

		//shader_bind_attributes(shader);

		// draw vertex_array bound to our mesh
		va_draw(a->mesh->array);

		//shader_unbind_attributes(shader);
	}
}


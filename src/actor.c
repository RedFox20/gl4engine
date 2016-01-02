#include "actor.h"
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
		affine_transform(a, &model);
		shader_bind_mat_mvp(shader, viewProjection, &model);
		shader_bind_tex_diffuse(shader, texture->glTexture);

		shader_bind_attributes(shader);

		// draw vertex_array bound to our mesh
		va_draw(a->mesh->array);

		shader_unbind_attributes(shader);
	}
}


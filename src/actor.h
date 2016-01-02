#pragma once
#include <stdbool.h>
#include "types3d.h"
#include "mesh.h"
#include "material.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct Actor // definition of a 3D actor in our scenes
{
	vec3 pos; // XYZ position
	vec3 rot; // euler XYZ rotation
	vec3 scale; // XYZ scale

	StaticMesh*  mesh;      // STRONG REF: 3D mesh
	Material     material;  // STRONG REF: material (color, shader, texture)

} Actor;

////////////////////////////////////////////////////////////////////////////////

// default initializes an actor
void actor_init(Actor* a);
// destroy an actor frees model and texture data
void actor_destroy(Actor* a);
// safely removes reference to mesh
void actor_clear_mesh(Actor* a);
// safely removes reference to material
void actor_clear_material(Actor* a);

// takes ownership of the provides mesh
bool actor_mesh(Actor* a, StaticMesh* mesh);
// takes ownership of the provided material
bool actor_material(Actor* a, Material* mat);

////////////////////////////////////////////////////////////////////////////////

// draws this model in the specified viewprojection
// and in the context of an already bound shader
void actor_draw(Actor* a, const mat4* viewProjection);

////////////////////////////////////////////////////////////////////////////////
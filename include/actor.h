#pragma once
#include <stdbool.h>
#include "types3d.h"
#include "mesh.h"
#include "material.h"

////////////////////////////////////////////////////////////////////////////////

struct World;
struct Actor;
typedef void (*ActorTick)(struct World* world, struct Actor* actor, double deltaTime);

typedef struct Actor // definition of a 3D actor in our scenes
{
	char name[32]; // unique actor name

	vec3 pos;   // XYZ position
	vec3 rot;   // euler XYZ rotation
	vec3 scale; // XYZ scale

	StaticMesh*  mesh;      // STRONG REF: 3D mesh
	Material     material;  // STRONG REF: material (color, shader, texture)

	ActorTick tick; // custom TICK function

} Actor;

////////////////////////////////////////////////////////////////////////////////

// creates a new default initialized actor with the given name
Actor* actor_create(const char* name);
// destroys an allocated actor
void actor_destroy(Actor*);

// default initializes an actor with the given name
void actor_init(Actor* a, const char* name);
// clears frees model and texture data
void actor_clear(Actor* a);
// safely removes reference to mesh
void actor_clear_mesh(Actor* a);
// safely removes reference to material
void actor_clear_material(Actor* a);

// takes ownership of the provides mesh
bool actor_mesh(Actor* a, StaticMesh* mesh);
// takes ownership of the provided material
bool actor_material(Actor* a, Material* mat);

////////////////////////////////////////////////////////////////////////////////

// gets the affine transformation matrix of this actor
void actor_affine_matrix(mat4* out, const Actor* a);

// draws this model in the specified viewprojection
// and in the context of an already bound shader
void actor_draw(Actor* a, const mat4* viewProjection);

////////////////////////////////////////////////////////////////////////////////
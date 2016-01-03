#pragma once
#include <GL/glfw3.h>
#include "actor.h"
#include "vector.h"

typedef struct Camera // camera inherits from Actor, does not have any model
{
	Actor a;
	vec3 target;     // camera look target
	bool use_target; // true: use lookAt target
} Camera;

typedef struct World
{
	// general world context:
	float width, height; // current framebuffer width/height
	double deltaTime;    // deltaTime since last frame
	GLFWwindow* window;  // GLFW window
	
	// event callbacks:
	void (*frame_tick)(struct World* world, double deltaTime); // REQUIRED (!!)
	void (*begin_play)(struct World* world); // optional
	void (*end_play)(struct World* world);   // optional

	// resource management pools:
	ShaderManager* shaderMgr;  // shader resource pool
	MeshManager*   meshMgr;    // mesh resource pool
	TexManager*    textureMgr; // texture resource pool

	Camera* camera;            // current camera actor
	Camera  defaultCamera;     // default camera actor

	pvector actors;            // vector<Actor*> all actors present in the World

} World;

////////////////////////////////////////////////////////////////////////////////

void world_create(World* world);
void world_destroy(World* world);
void world_main_loop(World* world, GLFWwindow* window);

Actor*      world_create_actor(World* world, const char* name);
Actor*      world_find_actor(World* world,   const char* name);
Shader*     world_load_shader(World* world,  const char* shaderPath);
StaticMesh* world_load_mesh(World* world,    const char* modelPath);
Texture*    world_load_texture(World* world, const char* texturePath);
Material    world_load_material(World* world, const char* shaderPath, const char* texturePath);


#include "world.h"
#include <stdlib.h>
#include <string.h>
#include "util.h"

////////////////////////////////////////////////////////////////////////////////

void world_create(World* world)
{
	memset(world, 0, sizeof(*world));

	world->camera = &world->defaultCamera;
	actor_init(&world->defaultCamera.a, "defaultCamera");
	pvector_create(world->actors.vec);
}

void world_destroy(World* world)
{
	Actor** it  = pvector_begin(&world->actors, Actor);
	Actor** end = pvector_end(&world->actors, Actor);
	for (; it != end; ++it)
		actor_destroy(*it);

	actor_clear(&world->defaultCamera.a);
	pvector_destroy(world->actors.vec);

	if (world->meshMgr)    ires_manager_destroy(world->meshMgr);
	if (world->textureMgr) ires_manager_destroy(world->textureMgr);
	if (world->shaderMgr)  ires_manager_destroy(world->shaderMgr);
}

////////////////////////////////////////////////////////////////////////////////

static void update_screen_size(World* world, GLFWwindow* window)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	world->width  = (float)width;
	world->height = (float)height;
}

void world_main_loop(World* world, GLFWwindow* window)
{
	// init values
	glfwSetWindowUserPointer(window, world);
	update_screen_size(world, window);
	world->deltaTime = 0.0;
	world->window = window;

	// main loop has begun
	if (world->begin_play) 
		world->begin_play(world);

	while (!glfwWindowShouldClose(window))
	{
		double deltaTime = world->deltaTime = timer_elapsed_vsync(60.0);  // VSYNC framerate to 60fps
		update_screen_size(world, window);
		{
			//////// Update Actor tick ////////
			int count      = world->actors.size;
			Actor** actors = world->actors.data;
			for (int i = 0; i < count; ++i) {
				Actor* a = actors[i];
				if (a->tick) {
					a->tick(world, a, deltaTime);
				}
			}

			//////// Render tick ////////
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			world->frame_tick(world, world->deltaTime);
			glfwSwapBuffers(window);
		}
		glfwPollEvents();
	}

	// main loop has finished
	if (world->end_play)
		world->end_play(world);
}

////////////////////////////////////////////////////////////////////////////////

Actor* world_create_actor(World* world, const char* name)
{
	Actor* actor = actor_create(name);
	pvector_append(world->actors.vec, actor);
	return actor;
}

Actor* world_find_actor(World* world, const char* name)
{
	Actor** it = pvector_data(world->actors.vec, Actor);
	Actor** end = pvector_end(world->actors.vec, Actor);
	for (; it != end; ++it)
		if (strcmp((*it)->name, name) == 0)
			return *it;
	return NULL;
}

Shader* world_load_shader(World* world, const char* modelPath)
{
	if (!world->shaderMgr)
		world->shaderMgr = shader_manager_create(32);
	return (Shader*)iresource_load(world->shaderMgr, modelPath);
}
StaticMesh* world_load_mesh(World* world, const char* modelPath)
{
	if (!world->meshMgr)
		world->meshMgr = mesh_manager_create(64);
	return (StaticMesh*)iresource_load(world->meshMgr, modelPath);
}
Texture* world_load_texture(World* world, const char* modelPath)
{
	if (!world->textureMgr)
		world->textureMgr = tex_manager_create(64);
	return (Texture*)iresource_load(world->textureMgr, modelPath);
}
Material world_load_material(World* world, const char* shaderPath, const char* texturePath)
{
	return material_create(world_load_shader(world, shaderPath), 
		                   world_load_texture(world, texturePath));
}
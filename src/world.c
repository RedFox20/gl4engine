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
	pvector_create(&world->actors);
}

void world_destroy(World* world)
{
	Actor** it = pvector_begin(&world->actors, Actor);
	Actor** end = pvector_end(&world->actors, Actor);
	for (; it != end; ++it)
		actor_destroy(*it);

	actor_clear(&world->defaultCamera.a);
	pvector_destroy(&world->actors);

	if (world->meshMgr)    res_manager_destroy(&world->meshMgr->rm);
	if (world->textureMgr) res_manager_destroy(&world->textureMgr->rm);
	if (world->shaderMgr)  res_manager_destroy(&world->shaderMgr->rm);
}

////////////////////////////////////////////////////////////////////////////////

static void update_screen_size(World* world, GLFWwindow* window)
{
	int width;
	int height;
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
		world->deltaTime = timer_elapsed_vsync(60.0);  // VSYNC framerate to 60fps
		update_screen_size(world, window);
		{
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
	pvector_append(&world->actors, actor);
	return actor;
}

Actor* world_find_actor(World* world, const char* name)
{
	Actor** it = pvector_data(&world->actors, Actor);
	Actor** end = pvector_end(&world->actors, Actor);
	for (; it != end; ++it)
		if (strcmp((*it)->name, name) == 0)
			return *it;
	return NULL;
}

Shader* world_load_shader(World* world, const char* modelPath)
{
	if (!world->shaderMgr)
		world->shaderMgr = shader_manager_create(32);
	return (Shader*)resource_load(&world->shaderMgr->rm, modelPath);
}
StaticMesh* world_load_mesh(World* world, const char* modelPath)
{
	if (!world->meshMgr)
		world->meshMgr = mesh_manager_create(64);
	return (StaticMesh*)resource_load(&world->meshMgr->rm, modelPath);
}
Texture* world_load_texture(World* world, const char* modelPath)
{
	if (!world->textureMgr)
		world->textureMgr = tex_manager_create(64);
	return (Texture*)resource_load(&world->textureMgr->rm, modelPath);
}
Material world_load_material(World* world, const char* shaderPath, const char* texturePath)
{
	return material_create(world_load_shader(world, shaderPath), 
		                   world_load_texture(world, texturePath));
}
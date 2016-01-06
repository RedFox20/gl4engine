#include <GL/glew.h>
#include <GL/glfw3.h>
#include "main.h"
#include "util.h"
#include "world.h"

//////////////////////////////////////////////////////////////////////////////////

static vec3 UP = { 0.0f, 1.0f, 0.0f }; // OpenGL: Y is up

static void frame_tick(World* w, double deltaTime)
{
	Camera* c = w->camera;
	mat4 proj = mat4_perspective(45.0f, w->width, w->height, 0.1f, 10000.0f);
	mat4 look = mat4_lookat(c->a.pos, c->target, UP);
	mat4_mul(&proj, &look); // viewprojMatrix = proj * view

	//mat4 view = proj;
	//mat4_identity(&view);
	{
		// render 3d scene
		Actor** actors = w->actors.data;
		for (int i = 0; i < w->actors.size; ++i)
		{
			actor_draw(actors[i], &proj);
		}
	}

	proj = mat4_ortho(0.0f, w->width, 0.0f, w->height);
	{
		// render user interface (if any)

	}
}

static void statue_tick(World* world, Actor* a, double deltaTime)
{
	GLFWwindow* w = world->window;
	vec3  dp = vec3_ZERO;
	vec3  dr = vec3_ZERO;
	float ds = 0.0f;
	float dt = (float)deltaTime;

	if (glfwGetKey(w, 'A')) dp.x += 5 * dt; // 5 coord/s
	if (glfwGetKey(w, 'R')) dp.y += 5 * dt;
	if (glfwGetKey(w, 'W')) dp.z += 5 * dt;
	if (glfwGetKey(w, 'D')) dp.x -= 5 * dt;
	if (glfwGetKey(w, 'F')) dp.y -= 5 * dt;
	if (glfwGetKey(w, 'S')) dp.z -= 5 * dt;
	if (glfwGetKey(w, 'Q')) dr.y += 120 * dt; // 120 deg/s
	if (glfwGetKey(w, 'E')) dr.y -= 120 * dt; // 120 deg/s
	if (glfwGetKey(w, 'Z')) ds = +1 * dt;
	if (glfwGetKey(w, 'C')) ds = -1 * dt;
	a->pos   = vec3_add(a->pos, dp);
	a->rot   = vec3_add(a->rot, dr);
	a->scale = vec3_addf(a->scale, ds);
	printf("statue %.2f %.2f %.2f | %.0f %.0f %.0f | %.1f\n", 
		a->pos.x,a->pos.y,a->pos.z,  a->rot.x, a->rot.y, a->rot.z, a->scale.x);
}

static Actor* set_actor_mesh(World* world, Actor* actor, const char* meshName)
{
	actor_clear(actor);
	if (actor_mesh(actor, world_load_mesh(world, meshName)))
	{
		actor->material = world_load_material(world, 
			"shaders/simple", actor->mesh->model->tex_name);
	}
	return actor;
}

static void begin_play(World* world)
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);  // clear background to soft black

	world->camera->a.pos  = vec3_new(0, 9, -14);
	world->camera->target = vec3_new(0, 4, 0);

	Actor* statue = world_create_actor(world, "statue");
	set_actor_mesh(world, statue, "statue_mage.bmd");
	statue->tick = &statue_tick;
}

static void end_play(World* world)
{

}

//////////////////////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	World* world = glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
	(void)world;
}
void btn_callback(GLFWwindow* window, int button, int action, int mods)
{
	World* world = glfwGetWindowUserPointer(window);
	(void)world;
}
static void glfw_error(int err, const char* description)
{
	LOG("GLFW error %d: %s\n", err, description);
}

int main(int argc, char** argv)
{
	//////////////// Init GLFW /////////////
	glfwSetErrorCallback(&glfw_error);
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // disable deprecated
	glfwWindowHint(GLFW_SAMPLES, 4); // 4xMSAA
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE); // DEBUG OpenGL Driver

	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL 4.3 with GLFW", NULL, NULL);
	if (!window) {
		glfwTerminate();
		sleep_ms(2000);
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &key_callback);
	glfwSetMouseButtonCallback(window, &btn_callback);
	
	////////////// Init GLEW /////////////
	glewExperimental = true; // enable loading experimental OpenGL features
	GLenum status = glewInit();
	if (status != GLEW_OK) { // init GL extension wrangler
		LOG("GLEW error: %s\n", glewGetErrorString(status));
		sleep_ms(2000);
		return EXIT_FAILURE;
	}

	////////////// init basic OpenGL //////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); 	
	glEnable(GL_DEPTH_TEST); // important!

	///////// create world ////////
	World world;
	world_create(&world);
	{
		world.frame_tick = &frame_tick;
		world.begin_play = &begin_play;
		world.end_play   = &end_play;
		world_main_loop(&world, window);
	}
	world_destroy(&world);
	///////////////////////////////

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////

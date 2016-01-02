#include <GL/glew.h>
#include <GL/glfw3.h>
#include "main.h"
#include "util.h"
#include "actor.h"
#include "shader.h"


typedef struct World
{
	float width, height; // current framebuffer width/height
	double deltaTime;    // deltaTime since last frame

	vec3 camPos;    // camera position
	vec3 camTarget; // camera look target

	ShaderManager* shaders;  // shader resource pool
	MeshManager*   meshes;   // mesh resource pool
	TexManager*    textures; // texture resource pool

	int numActors;
	Actor actors[64];

} World;


//////////////////////////////////////////////////////////////////////////////////

void world_tick(World* w, double deltaTime)
{
	static vec3 UP = { 0.0f, 1.0f, 0.0f }; // OpenGL: Y is up

	mat4 proj = mat4_perspective(45.0f, w->width, w->height, 1.0f, 1000.0f);
	mat4 view = mat4_lookat(w->camPos, w->camTarget, UP);
	mat4_mul(&proj, &view); // proj(viewprojMatrix) = proj * view
	{
		// render 3d scene
		for (int i = 0; i < w->numActors; ++i)
		{
			Actor* a = &w->actors[i];
			actor_draw(a, &view);
		}
	}

	proj = mat4_ortho(0.0f, w->width, 0.0f, w->height);
	{
		// render user interface (if any)

	}
}

//////////////////////////////////////////////////////////////////////////////////

void world_create(World* w)
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);  // clear background to soft black

	w->camPos    = vec3_new(0, 9, -14);
	w->camTarget = vec3_new(0, 4, 0);
	w->shaders  = shader_manager_create(16);
	w->meshes   = mesh_manager_create(64);
	w->textures = tex_manager_create(64);

	Actor* a = &w->actors[w->numActors++];
	actor_init(a);
	if (actor_mesh(a, mesh_load(w->meshes, "statue_mage.bmd")))
	{
		Material m = material_from_file(w->shaders, "shaders/simple", 
			                            w->textures, a->mesh->model->tex_name);
		actor_material(a, &m);
	}
}

//////////////////////////////////////////////////////////////////////////////////

void world_destroy(World* w)
{
	for (int i = 0; i < w->numActors; ++i)
		actor_destroy(&w->actors[i]);
	w->numActors = 0;
	tex_manager_destroy(w->textures);
	mesh_manager_destroy(w->meshes);
	shader_manager_destroy(w->shaders);
}

//////////////////////////////////////////////////////////////////////////////////

void main_loop(World* w, GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, w);
	while (!glfwWindowShouldClose(window))
	{
		w->deltaTime = timer_elapsed_vsync(60.0);  // VSYNC framerate to 60fps

		int fbw, fbh; // update screen size:
		glfwGetFramebufferSize(window, &fbw, &fbh);
		w->width = (float)fbw, w->height = (float)fbh;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear background

		world_tick(w, w->deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	World* w = glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	(void)w->deltaTime;
}
static void glfw_error(int err, const char* description)
{
	LOG("GLFW error %d: %s\n", err, description);
}

void glfw_settings() {

}

int main(int argc, char** argv)
{
	//////////////// Init GLFW /////////////
	glfwSetErrorCallback(&glfw_error);
	if (!glfwInit())
		return EXIT_FAILURE;

	// do we even need these?
	// changing one of these will reset context
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_SAMPLES, 2); // 2xMSAA
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE); // DEBUG OpenGL Driver
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // disable deprecated

	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL 3.3 with GLFW", NULL, NULL);
	if (!window) {
		glfwTerminate();
		sleep_ms(2000);
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &key_callback);
	
	////////////// Init GLEW /////////////
	glewExperimental = true; // enable loading experimental OpenGL features
	GLenum status = glewInit();
	if (status != GLEW_OK) { // init GL extension wrangler
		LOG("GLEW error: %s\n", glewGetErrorString(status));
		sleep_ms(2000);
		return EXIT_FAILURE;
	}

	////////////// init basic OpenGL //////////////
	//glFrontFace(GL_CW); // CCW:default
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); 	

	// enter world
	World world = { 0 };
	world_create(&world);
	main_loop(&world, window);
	world_destroy(&world);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////

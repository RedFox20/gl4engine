#include <GL/glew.h>
#include <GL/glfw3.h>
#include "main.h"
#include "util.h"
#include "actor.h"
#include "shader.h"


typedef struct World
{
	int width, height; // current framebuffer width/height
	double deltaTime;  // deltaTime since last frame

	vec3 camPos;    // camera position
	vec3 camTarget; // camera look target

	ShaderManager* shaders;  // shader resource pool
	MeshManager*   meshes;   // mesh resource pool
	TexManager*    textures; // texture resource pool

	int numActors;
	Actor actors[64];

} World;


//////////////////////////////////////////////////////////////////////////////////

void world_tick(World* s, double deltaTime)
{
	static vec3 UP = { 0.0f, 1.0f, 0.0f }; // OpenGL: Y is up

	mat4 proj = mat4_perspective(45.0f, (float)s->width, (float)s->height, 1.0f, 1000.0f);
	mat4 view = mat4_lookat(s->camPos, s->camTarget, UP);
	mat4_mul(&proj, &view); // proj(viewprojMatrix) = proj * view
	{
		// render 3d scene

	}

	proj = mat4_ortho(0.0f, (float)s->width, 0.0f, (float)s->height);
	{
		// render user interface (if any)

	}
}

//////////////////////////////////////////////////////////////////////////////////

void world_create(World* s)
{
	s->shaders  = shader_manager_create(16);
	s->meshes   = mesh_manager_create(64);
	s->textures = tex_manager_create(64);

	Actor* a = &s->actors[s->numActors++];
	actor_init(a);
	if (actor_mesh(a, mesh_load(s->meshes, "statue_mage.bmd")))
	{
		Material m = material_from_file(s->shaders, "shaders/simple", 
			                            s->textures, a->mesh->model->TexName);
		actor_material(a, &m);
	}
}

//////////////////////////////////////////////////////////////////////////////////

void world_destroy(World* s)
{
	for (int i = 0; i < s->numActors; ++i)
		actor_destroy(&s->actors[i]);
	s->numActors = 0;
	tex_manager_destroy(s->textures);
	mesh_manager_destroy(s->meshes);
	shader_manager_destroy(s->shaders);
}

//////////////////////////////////////////////////////////////////////////////////

void main_loop(World* s, GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, s);
	while (!glfwWindowShouldClose(window))
	{
		s->deltaTime = timer_elapsed_vsync(60.0);  // VSYNC framerate to 60fps
		glfwGetFramebufferSize(window, &s->width, &s->height);

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);  // clear background to soft black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear background

		world_tick(s, s->deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

//////////////////////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	World* s = glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	(void)s->deltaTime;
}

static void glfw_error(int err, const char* description)
{
	LOG("GLFW error %d: %s\n", err, description);
}

int main(int argc, char** argv)
{
	////////////// Init GLFW /////////////
	glfwSetErrorCallback(&glfw_error);
	if (!glfwInit())
		return EXIT_FAILURE;
	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL 3.0 with GLFW", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &key_callback);

	////////////// Init GLEW /////////////
	glewExperimental = true; // enable loading experimental OpenGL features
	GLenum status = glewInit();
	if (status != GLEW_OK) { // init GL extension wrangler
		LOG("GLEW error: %s\n", glewGetErrorString(status));
		return EXIT_FAILURE;
	}

	////////////// init basic OpenGL //////////////
	glEnable(GL_TEXTURE_2D); // enable 2d texturing for 3d models
	glEnable(GL_DEPTH_TEST);  // enable depth testing
	glEnable(GL_BLEND); 	  // enable alpha mapping
	glClearDepth(1.0f);	      // depth buffer setup
	glFrontFace(GL_CCW);      // set face rotation
	glShadeModel(GL_SMOOTH);  // enable smooth shading
	glDepthFunc(GL_LEQUAL);   // depthtest less-than-equal
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); 	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 

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
const char* ForceString = "~~ The Force Awakens ~~";
//////////////////////////////////////////////////////////////////////////////////

#include "user/mesh.h"
#include "user/model_loader.h"
#include "user/node3d.h"
#include "user/physics_node3d.h"
#include "user/point_light.h"
#include "user/skybox.h"
#include <iostream>
#include <string>
#include <glm/gtx/string_cast.hpp>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

using namespace std;

const unsigned int width = 1600;
const unsigned int height = 1600;

btDiscreteDynamicsWorld *init_physics();
GLFWwindow *init_renderer(int width, int height, const char *title);
unsigned int init_ubo_camera_matrices();
unsigned int init_ubo_point_lights(int light_count);
void update_ubo_camera_matrices(unsigned int ubo_camera_matrices, Camera &camera);
void update_ubo_point_lights(unsigned int ubo_point_lights, PointLight *lights[], int light_count);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
glm::mat4 update_light_projection(glm::vec3 lightPos);
void sort_transparent_objects(vector<PhysicsNode3D *> &objects, Camera &camera);

Camera *main_camera;

int main()
{
	btDiscreteDynamicsWorld *dynamicsWorld = init_physics();

	GLFWwindow *window = init_renderer(width, height, "Bullet Physics");

	vector<std::string> faces{
		"../res/textures/skybox2/right.jpg",
		"../res/textures/skybox2/left.jpg",
		"../res/textures/skybox2/top.jpg",
		"../res/textures/skybox2/bottom.jpg",
		"../res/textures/skybox2/front.jpg",
		"../res/textures/skybox2/back.jpg"};

	Skybox skybox = Skybox(faces);
	cout << "Skybox created" << endl;
	// Texture textures[]{
	//	Texture("../res/textures/planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
	Texture("../res/textures/planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE);
	//};

	// Compile and link the vertex and frag shaders into shaderprogram
	Shader guitar_shader("../res/shaders/def.vert", "../res/shaders/def.frag");
	Shader level_shader("../res/shaders/def.vert", "../res/shaders/def.frag");
	Shader monkey_shader("../res/shaders/def.vert", "../res/shaders/def.frag");
	Shader shadow_map_shader("../res/shaders/shadow_map.vert", "../res/shaders/shadow_map.frag");
	Shader transparent_shader("../res/shaders/def.vert", "../res/shaders/blended.frag");
	Shader reflect_shader("../res/shaders/def.vert", "../res/shaders/def_reflect.frag");

	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	PhysicsNode3D guitar = PhysicsNode3D(glm::vec3(0.0f, 20.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), guitar_shader, "../res/models/guitar.glb", 1.0f);
	PhysicsNode3D monkey = PhysicsNode3D(glm::vec3(2.0f, 5.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), monkey_shader, "../res/models/monkey.dae", 1.0f);
	PhysicsNode3D monkey_reflect = PhysicsNode3D(glm::vec3(0.0f, 5.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), reflect_shader, "../res/models/monkey.dae", 1.0f);

	// PhysicsNode3D guitar = PhysicsNode3D(glm::vec3(8.0f, 5.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(f, 1.0f, 1.0f), guitar_shader, "../res/models/guitar.glb", 1.0f);

	PhysicsNode3D level = PhysicsNode3D(glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), level_shader, "../res/models/castle.obj", 0.0f, false);
	PhysicsNode3D transblue = PhysicsNode3D(glm::vec3(3.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), transparent_shader, "../res/models/transblue.glb", 1.0f, false);
	PhysicsNode3D transorange = PhysicsNode3D(glm::vec3(2.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), transparent_shader, "../res/models/transorange.glb", 1.0f, false);

	vector<PhysicsNode3D *> transparent_objects = {&transblue, &transorange};

	// PhysicsNode3D monkey = PhysicsNode3D(glm::vec3(0.0f, 10.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), monkey_shader, "../res/models/monkey.dae", 1.0f);

	// PhysicsNode3D level = PhysicsNode3D(glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.001f, 0.001f, 0.001f), level_shader, "../res/models/terrain1.fbx", 0.0f);

	dynamicsWorld->addRigidBody(level.rigid_body);
	dynamicsWorld->addRigidBody(guitar.rigid_body);
	dynamicsWorld->addRigidBody(monkey.rigid_body);
	dynamicsWorld->addRigidBody(monkey_reflect.rigid_body);
	dynamicsWorld->addRigidBody(transblue.rigid_body);
	dynamicsWorld->addRigidBody(transorange.rigid_body);

	PointLight point_light = PointLight(glm::vec3(0.0f, -7.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), .1, 0.01f, 0.001f, "../res/models/lightball.dae");
	PointLight point_light2 = PointLight(glm::vec3(1.0f, -8.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), .1, 0.01f, 0.001f, "../res/models/lightball.dae");

	cout << "objects created" << endl;
	const int light_count = 2;
	PointLight *lights[light_count] = {
		&point_light,
		&point_light2};

	unsigned int ubo_camera_matrices = init_ubo_camera_matrices();
	unsigned int ubo_point_lights = init_ubo_point_lights(light_count);

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
	camera.update_matrix(70.0f, 0.1f, 1000.0f);
	main_camera = &camera;
	cout << "camera created" << endl;

	unsigned int shadow_map_fbo;
	glGenFramebuffers(1, &shadow_map_fbo);

	unsigned int shadow_map_width = 1024;
	unsigned int shadow_map_height = 1024;
	unsigned int shadow_map_texture;
	glGenTextures(1, &shadow_map_texture);
	glBindTexture(GL_TEXTURE_2D, shadow_map_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_width, shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clamp_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);

	glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::mat4 lightProjection = update_light_projection(lightPos);

	shadow_map_shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadow_map_shader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		level.set_cubemap(skybox.texture_id);

		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			// Make pointlight cycle between red and blue
			// point_light.light_color = glm::vec4(1.0f + 0.5f * sin(crntTime), 0.5f, 1.0f - 1.0f * sin(crntTime), 1.0f);
			// point_light2.light_color = glm::vec4(0.5f, 1.0f - 1.0f * sin(crntTime), 1.0f + 0.5f * sin(crntTime), 1.0f);

			// Make pointlight fly back and forth
			point_light.set_position(glm::vec3(point_light.position.x - 5 * cos(crntTime) / 60.0f, point_light.position.y + 2 * sin(crntTime) / 60.0f, point_light.position.z + 5 * sin(crntTime) / 60.0f));
			point_light2.set_position(glm::vec3(point_light2.position.x + 5 * cos(crntTime) / 60.0f, point_light2.position.y - 2 * sin(crntTime) / 60.0f, point_light2.position.z - 5 * sin(crntTime) / 60.0f));
			// lightPos.x += 5 * sin(crntTime) / 6000.0f;
			// lightPos.z += 50 * cos(crntTime) / 6000.0f;
			//  cout << "Pointlight position: " << glm::to_string(point_light.position) << endl;
			prevTime = crntTime;
		}

		camera.inputs(window);
		camera.update_matrix(70.0f, 0.1f, 1000.0f);

		update_ubo_camera_matrices(ubo_camera_matrices, camera);
		update_ubo_point_lights(ubo_point_lights, lights, light_count);

		dynamicsWorld->stepSimulation(1 / 60.0f, 10, 1 / 60.0f);

		guitar.update();
		level.update();
		monkey.update();
		monkey_reflect.update();
		transblue.update();
		transorange.update();

		// glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_TEST);

		// glEnable(GL_CULL_FACE);
		//  glCullFace(GL_FRONT);

		// Preparations for the Shadow Map
		glViewport(0, 0, shadow_map_width, shadow_map_height);
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		guitar.draw(shadow_map_shader, camera);
		level.draw(shadow_map_shader, camera);
		monkey.draw(shadow_map_shader, camera);
		monkey_reflect.draw(shadow_map_shader, camera);
		point_light.draw(shadow_map_shader, camera);
		point_light2.draw(shadow_map_shader, camera);
		transblue.draw(shadow_map_shader, camera);
		transorange.draw(shadow_map_shader, camera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glCullFace(GL_BACK);
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		// model.Draw(shadowMapProgram, camera);
		lightProjection = update_light_projection(lightPos);

		guitar.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);
		level.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);
		monkey.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);
		monkey_reflect.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);

		point_light.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);
		point_light2.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);

		transblue.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);
		transorange.update_shadow_uniforms(lightProjection, lightPos, lightColor, shadow_map_texture);

		guitar.draw();
		level.draw();
		monkey.draw();
		monkey_reflect.draw();
		point_light.draw();
		point_light2.draw();
		sort_transparent_objects(transparent_objects, camera);
		glEnable(GL_BLEND);
		for (PhysicsNode3D *object : transparent_objects)
		{
			object->draw();
		}
		glDisable(GL_BLEND);

		skybox.draw(camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete dynamicsWorld;
	guitar.free();
	level.free();
	monkey.free();
	point_light.free();
	point_light2.free();
	guitar_shader.Delete();
	level_shader.Delete();
	monkey_shader.Delete();
	shadow_map_shader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void sort_transparent_objects(vector<PhysicsNode3D *> &objects, Camera &camera)
{
	std::sort(objects.begin(), objects.end(), [&camera](PhysicsNode3D *a, PhysicsNode3D *b)
			  {
		float distance_a = glm::length(camera.position - a->position);
		float distance_b = glm::length(camera.position - b->position);
		return distance_a > distance_b; });
}

glm::mat4 update_light_projection(glm::vec3 lightPos)
{
	glm::mat4 orthogonal = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return orthogonal * lightView;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	main_camera->width = width;
	main_camera->height = height;
	main_camera->update_matrix(70.0f, 0.1f, 1000.0f);
}

void update_ubo_camera_matrices(unsigned int ubo_camera_matrices, Camera &camera)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_camera_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera.camera_matrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_camera_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void update_ubo_point_lights(unsigned int ubo_point_lights, PointLight *lights[], int light_count)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_point_lights);
	for (int i = 0; i < light_count; i++)
	{
		// cout << "Updating light " << i << endl;
		// cout << "Position: " << glm::to_string(lights[i].position) << endl;
		size_t base_offset = i * (sizeof(glm::vec3) + sizeof(glm::vec4) + 3 * sizeof(float) + 8); // 12 bytes padding
		glBufferSubData(GL_UNIFORM_BUFFER, base_offset, sizeof(glm::vec3), glm::value_ptr(glm::vec3(lights[i]->position.x, lights[i]->position.y, lights[i]->position.z)));
		glBufferSubData(GL_UNIFORM_BUFFER, base_offset + 16, sizeof(glm::vec4), glm::value_ptr(lights[i]->light_color));
		glBufferSubData(GL_UNIFORM_BUFFER, base_offset + 32, sizeof(float), &lights[i]->linear);
		glBufferSubData(GL_UNIFORM_BUFFER, base_offset + 36, sizeof(float), &lights[i]->quadratic);
		glBufferSubData(GL_UNIFORM_BUFFER, base_offset + 40, sizeof(float), &lights[i]->constant);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

unsigned int init_ubo_camera_matrices()
{
	unsigned int ubo_camera_matrices;
	glGenBuffers(1, &ubo_camera_matrices);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_camera_matrices);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_camera_matrices, 0, sizeof(glm::mat4) + sizeof(glm::vec3));
	return ubo_camera_matrices;
}

unsigned int init_ubo_point_lights(int light_count)
{

	// Offset for each element in the buffer.
	size_t position_offset = 0;	  // Offset 0
	size_t color_offset = 16;	  // Offset 16
	size_t linear_offset = 32;	  // Offset 32
	size_t quadratic_offset = 36; // Offset 36
	size_t constant_offset = 40;  // Offset 40
	size_t size = (position_offset + color_offset + linear_offset + quadratic_offset + constant_offset) * light_count;
	unsigned int ubo_point_lights;
	glGenBuffers(1, &ubo_point_lights);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_point_lights);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo_point_lights, 0, size);
	return ubo_point_lights;
}

btDiscreteDynamicsWorld *init_physics()
{
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld *dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -5.0, 0)); // Set gravity
	return dynamicsWorld;
}

GLFWwindow *init_renderer(int width, int height, const char *title)
{
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//// Create a window
	GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Can't create glfw window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	gladLoadGL();

	// Set the viewport, and a bg color -> then swap the buffers
	glViewport(0, 0, width, height);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return window;
}
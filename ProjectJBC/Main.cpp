//modified from http://learnopengl.com/
//modified from example skeleton code 2019 winter comp371

#include "stdafx.h"
#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "OBJloader.h"  //include the object loader
#include "shaderloader.h"
#include "mapLoader.h"
#include <string>
using namespace std;

// Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;
GLFWwindow *window;

//Camera
glm::vec3 cam_pos = glm::vec3(0, 2, -5);
glm::vec3 cam_dir = glm::vec3(0, 0, 1);
glm::vec3 cam_up = glm::vec3(0, 1, 0);
//camera move speed
float camera_scroll_speed = 0.3;
float edge_scroll_region = 50; //in pixels
bool camera_scroll_continuous = true;

//module path
string module_path = "E:\\Project-Jean-Baptiste-Colbert\\Modules\\Native\\";

//Model
glm::vec3 transl = glm::vec3(0, 0, 0);

//rotation angles
int rotateX = 0;
int rotateY = 0;
int rotateZ = 0;
//used to track the mouse position for zoom
double xInit, yInit;
//used to track if mouse buttun is pressed
bool drag = false;
//the rotation angle of the camera about the up axis
float cam_rotZ = 0;
//the rotation angle of the camera about its x axis
float cam_rotX = 0;
//the scale of the object
float scale = 1;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		cam_pos += cam_dir;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		cam_pos -= cam_dir;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		cam_pos += glm::cross(cam_up, cam_dir);
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		cam_pos -= glm::cross(cam_up, cam_dir);
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		transl.y += 1;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		transl.y -= 1;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		transl.x += 1;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		transl.x -= 1;
	}
	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
		transl.z += 1;
	}
	if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
		transl.z -= 1;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		rotateX += 10;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		rotateY += 10;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		rotateZ += 10;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		cam_rotZ += 0.174533; //10 degrees in radians
		cam_dir = glm::vec3(sin(cam_rotZ)*cos(cam_rotX), sin(cam_rotX), cos(cam_rotZ)*cos(cam_rotX)); // I spent 3 hours drawing circles to come up with this
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		cam_rotZ -= 0.174533;
		cam_dir = glm::vec3(sin(cam_rotZ)*cos(cam_rotX), sin(cam_rotX), cos(cam_rotZ)*cos(cam_rotX));
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cam_rotX += 0.174533;
		cam_dir = glm::vec3(sin(cam_rotZ)*cos(cam_rotX), sin(cam_rotX), cos(cam_rotZ)*cos(cam_rotX));
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		cam_rotX -= 0.174533;
		cam_dir = glm::vec3(sin(cam_rotZ)*cos(cam_rotX), sin(cam_rotX), cos(cam_rotZ)*cos(cam_rotX));
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		scale = scale * 1.1;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		scale = scale * 0.9;
	}
}
//https://www.glfw.org/docs/latest/input_guide.html#input_mouse_button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//sets the initial position of the mouse
		glfwGetCursorPos(window, &xInit, &yInit);
		drag = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		drag = false;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	cam_pos -= glm::vec3(0, 0, (-yoffset)*2);
}

void cursor_scroll(GLFWwindow* window) {
	double* xpos = new double -1;
	double* ypos = new double -1;
	glfwGetCursorPos(window, xpos, ypos);

	//which type of camera movement is being used
	if (camera_scroll_continuous) {
		//determine if the cursor is within n units of the screen border
		if (*xpos < edge_scroll_region || *ypos < edge_scroll_region || *ypos >(HEIGHT - edge_scroll_region) || *xpos >(WIDTH - edge_scroll_region)) {
			//the cursor is somewhere in the edge region
			//draw a direction vector from screen center to current cursor position
			// destination_point - start_point = direction vector
			glm::vec3 scroll_direction = glm::normalize(glm::vec3((WIDTH / 2) - *xpos, (HEIGHT / 2) - *ypos, 0)) * camera_scroll_speed;
			//add the direction vector to the camera position to move it in the desired direction
			cam_pos += scroll_direction;
		}
	}
	else {
		//determine how close the mouse is to the left and right side of the screen
		glm::vec3 translateX = glm::vec3(0, 0, 0);
		glm::vec3 translateY = glm::vec3(0, 0, 0);

		if (*xpos < 50) {
			//move camera left
			//translateX += glm::cross(cam_up, cam_dir);
			translateX += glm::vec3(1, 0, 0);
		}
		else if (*xpos > (WIDTH - 50)) {
			//move camera right
			//translateX -= glm::cross(cam_up, cam_dir);
			translateX += glm::vec3(-1, 0, 0);
		}

		//determine how close the mouse is to the top and bottom of the screen
		if (*ypos < 50) {
			//move camera down
			//translateY += cam_up;
			translateY += glm::vec3(0, 1, 0);
		}
		else if (*ypos > (HEIGHT - 50)) {
			//move camera up
			//translateY -= cam_up;
			translateY += glm::vec3(0, -1, 0);
		}
		cam_pos += camera_scroll_speed * (translateX + translateY);
	}
}

int init() {
	std::cout << "Starting GLFW context, OpenGL 4.3" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);

	//WINDOW
	window = glfwCreateWindow(WIDTH, HEIGHT, "Project JBC", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	if (init() != 0) {
		return EXIT_FAILURE;
	}
	
	string map_path = module_path + "Map\\";
	Map* map = LoadMap(map_path);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Build and compile our shader program
	// Vertex shader

	GLuint shader = loadSHADER("vertex.shader", "fragment.shader");
	glUseProgram(shader);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	//loadOBJ("cube.obj", vertices, normals, UVs); //read the vertices from the cube.obj file
	loadOBJ("cat.obj", vertices, normals, UVs);

	GLuint VAO;
	GLuint vertices_VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_VBO);
	glBindVertexArray(VAO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	glm::mat4 modl_matrix = glm::mat4(1.0f);
	//glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos+cam_dir, cam_up);
	glm::mat4 proj_matrix = glm::perspective(45.f, 1.f, 0.1f, 100.f);

	GLuint mm_loc = glGetUniformLocation(shader, "mm");
	GLuint vm_loc = glGetUniformLocation(shader, "vm");
	GLuint pm_loc = glGetUniformLocation(shader, "pm");


	glUniformMatrix4fv(pm_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));

	// Game loop
	
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		cursor_scroll(window);

		//rotations about the up axis are done by changing cam_dir
		glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);
		glUniformMatrix4fv(vm_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		//rotates the object
		glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), rotateX / 180.f, glm::vec3(1, 0, 0));
		rotator = rotator * glm::rotate(glm::mat4(1.0f), rotateY / 180.f, glm::vec3(0, 1, 0));
		rotator = rotator * glm::rotate(glm::mat4(1.0f), rotateZ / 180.f, glm::vec3(0, 0, 1));
		glm::mat4 translator = glm::translate(glm::mat4(1.0f), transl);
		glm::mat4 scalor = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f*scale, 1*scale, 1*scale));
		modl_matrix = translator * rotator * scalor;
		glUniformMatrix4fv(mm_loc, 1, GL_FALSE, glm::value_ptr(modl_matrix));
		
		glfwPollEvents();
		//glfwSetMousePos(1024 / 2, 768 / 2);
		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, vertices.size());
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
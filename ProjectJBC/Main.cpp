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
#include "map.h"
#include <string>
#include <chrono>
#include <cmath>
# define M_PI           3.14159265358979323846  /* pi */
using namespace std;

// Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;
GLFWwindow *window;

//light
GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 0.0 };
GLfloat light_position[] = { 0.0, 0.0, 6.0, 0.0 };

//Camera
glm::vec3 cam_pos = glm::vec3(0, 0, 5);
glm::vec3 cam_dir = glm::vec3(0, 0, -1);
glm::vec3 cam_up = glm::vec3(0, 1, 0);
double xpos_old = 0;
double ypos_old = 0;
glm::mat4 view_matrix;
//camera move speed
float camera_scroll_speed = 5.0;
float edge_scroll_region = 50; //in pixels
bool camera_scroll_continuous = true;
bool enable_edge_scroll = false;

//module path
string module_path = "E:\\Project-Jean-Baptiste-Colbert\\Modules\\Native\\";

//Model
glm::vec3 transl = glm::vec3(0, 0, 0);


//used to track the mouse position for zoom
double xInit, yInit;
//used to track if mouse buttun is pressed
bool drag = false;
//the rotation angle of the camera about the up axis
float cam_rotZ = 0;
//the rotation angle of the camera about its x axis
float cam_rotX = 0;


//TODO:
std::vector<glm::vec4> color_array_a;
GLuint VAO, VBO[2], cVBO[2]; //cVBO Color VBO

std::vector<glm::vec3> vertices_a;

//**** CALLBACK FUNCTIONS ****//

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS) {
		cam_pos += camera_scroll_speed * cam_up;

		/* TODO: note: code to modify color buffers at runtime.
		color_array_a.clear();
		color_array_a.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
		color_array_a.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
		color_array_a.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
		color_array_a.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
		glBindBuffer(GL_ARRAY_BUFFER, cVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, color_array_a.size() * sizeof(glm::vec4), &color_array_a.front(), GL_STATIC_DRAW);
		*/
	}
	if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS) {
		cam_pos -= camera_scroll_speed * cam_up;
	}
	if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		cam_pos += camera_scroll_speed * glm::cross(cam_up, cam_dir);
	}
	if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		cam_pos -= camera_scroll_speed * glm::cross(cam_up, cam_dir);
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		float radius = 1000 / (2 * M_PI);
		for (int i = 0; i < vertices_a.size(); i++) {
			float theta_x = (vertices_a[i].x / 1000) * 360;
			float theta_y = (vertices_a[i].y / 1000) * 360;

			vertices_a[i].x = sin(theta_x) * sin(theta_y);
			vertices_a[i].y = cos(theta_x);
			vertices_a[i].z = sin(theta_x) * cos(theta_y);

		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices_a.size() * sizeof(glm::vec3), &vertices_a.front(), GL_STATIC_DRAW);
	}
}

//www.glfw.org/docs/latest/input_guide.html#input_mouse_button
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
	cam_pos += glm::vec3(0, 0, (-yoffset)*2);
}

void cursor_scroll(GLFWwindow* window) {
	double* xpos = new double -1;
	double* ypos = new double -1;
	glfwGetCursorPos(window, xpos, ypos);

	if (enable_edge_scroll) {
		//which type of camera movement is being used
		if (camera_scroll_continuous) {
			//determine if the cursor is within n units of the screen border
			if (*xpos < edge_scroll_region || *ypos < edge_scroll_region || *ypos >(HEIGHT - edge_scroll_region) || *xpos >(WIDTH - edge_scroll_region)) {
				//the cursor is somewhere in the edge region
				//draw a direction vector from screen center to current cursor position
				// destination_point - start_point = direction vector
				glm::vec3 scroll_direction = glm::normalize(glm::vec3(*xpos - (WIDTH / 2), (HEIGHT / 2) - *ypos, 0)) * camera_scroll_speed;
				//add the direction vector to the camera position to move it in the desired direction

				cam_pos += scroll_direction;
			}
		}else {
			//determine how close the mouse is to the left and right side of the screen
			glm::vec3 translateX = glm::vec3(0, 0, 0);
			glm::vec3 translateY = glm::vec3(0, 0, 0);

			if (*xpos < 50) {
				//move camera left
				//translateX += glm::cross(cam_up, cam_dir);
				translateX += glm::vec3(-1, 0, 0);
			}
			else if (*xpos > (WIDTH - 50)) {
				//move camera right
				//translateX -= glm::cross(cam_up, cam_dir);
				translateX += glm::vec3(1, 0, 0);
			}

			//determine how close the mouse is to the top and bottom of the screen
			if (*ypos < 50) {
				//move camera down
				//translateY += cam_up;
				translateY += glm::vec3(0, -1, 0);
			}
			else if (*ypos > (HEIGHT - 50)) {
				//move camera up
				//translateY -= cam_up;
				translateY += glm::vec3(0, 1, 0);
			}
			cam_pos += camera_scroll_speed * (translateX + translateY);
		}
	}
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	//if (xpos_old == 0) {
	//	xpos_old = xpos;
	//	ypos_old = ypos;
	//}
	//float dx = xpos - xpos_old;
	//float dy = ypos - ypos_old;

	//glm::vec3  cam_right = glm::normalize(glm::cross(cam_dir, cam_up));
	//cam_dir += cam_right * (dx / 100.f);
	//cam_right = glm::normalize(glm::cross(cam_dir, cam_up));
	//cam_dir -= cam_up * (dy / 100.f);
	//cam_up = glm::normalize(glm::cross(cam_dir, -cam_right));

	//cam_dir = glm::normalize(cam_dir);

	//xpos_old = xpos;
	//ypos_old = ypos;
}
//**** END CALLBACK FUNCTIONS ****//

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
	//initialize game engine
	if (init() != 0) {
		return EXIT_FAILURE;
	}
	
	//initialize game map
	string map_path = module_path + "Map\\";
	Map* map = new Map(map_path);
	cout << "Map Load Complete" << endl;

	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
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

	
	std::vector<glm::vec3> vertices_b;
	//std::vector<glm::vec3> normals;
	//std::vector<glm::vec2> UVs;
	std::vector<unsigned int> indices_a;
	std::vector<unsigned int> indices_b;
	
	//ask map for vertices and indices.
	vertices_a = map->get_map_vertice_data();

	cout << "Vertices_a size: " << vertices_a.size() << endl;

	indices_a = map->get_map_indice_data();
	

	cout << "Vertices_a size: " << vertices_a.size() << endl;
	
	color_array_a = map->get_map_color_data();
	cout << "color_array_a size: " << color_array_a.size() << endl;
	
	//defined outside main function now
	//GLuint VAO[2], VBO[4], cVBO[2]; //cVBO Color VBO

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices_a.size() * sizeof(glm::vec3), &vertices_a.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_a.size() * sizeof(unsigned int), &indices_a.front(), GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);


	//ignore indices for loop test
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_a.size() * sizeof(unsigned int), &indices_a.front(), GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);

	//GLuint color_VBO;
	glGenBuffers(1, &cVBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, cVBO[0]);

	glBufferData(GL_ARRAY_BUFFER, color_array_a.size() * sizeof(glm::vec4), &color_array_a.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	
	//glBindVertexArray(VAO[1]);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//glBufferData(GL_ARRAY_BUFFER, vertices_b.size() * sizeof(glm::vec3), &vertices_b.front(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_b.size() * sizeof(unsigned int), &indices_b.front(), GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);

	//color for second object
	//GLuint color_VBO;
	//glGenBuffers(1, &cVBO[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, cVBO[1]);

	//std::vector<glm::vec4> color_array_b;
	//color_array_b.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
	//color_array_b.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
	//color_array_b.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
	//color_array_b.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));

	//glBufferData(GL_ARRAY_BUFFER, color_array_b.size() * sizeof(glm::vec4), &color_array_b.front(), GL_STATIC_DRAW);
	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	glm::mat4 modl_matrix = glm::mat4(1.0f);
	//glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos+cam_dir, cam_up);
	glm::mat4 proj_matrix = glm::perspective(45.f, 1.f, 0.1f, 1000.f);

	GLuint mm_loc = glGetUniformLocation(shader, "mm");
	GLuint vm_loc = glGetUniformLocation(shader, "vm");
	GLuint pm_loc = glGetUniformLocation(shader, "pm");


	glUniformMatrix4fv(pm_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));

	//timekeeping
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	int frames = 0;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		cursor_scroll(window);

		//rotations about the up axis are done by changing cam_dir
		glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);
		glUniformMatrix4fv(vm_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		//Variables for model matrix
		//rotation angles
		int rotateX = 0;
		int rotateY = 0;
		int rotateZ = 0;
		//the scale of the object
		float scale = 1;

		//rotates the object
		glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), rotateX / 180.f, glm::vec3(1, 0, 0));
		rotator = rotator * glm::rotate(glm::mat4(1.0f), rotateY / 180.f, glm::vec3(0, 1, 0));
		rotator = rotator * glm::rotate(glm::mat4(1.0f), rotateZ / 180.f, glm::vec3(0, 0, 1));
		glm::mat4 translator = glm::translate(glm::mat4(1.0f), transl);
		glm::mat4 scalor = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f*scale, 1*scale, 1*scale));
		modl_matrix = translator * rotator * scalor;
		glUniformMatrix4fv(mm_loc, 1, GL_FALSE, glm::value_ptr(modl_matrix));
		
		glfwPollEvents();
		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);


		//glDrawArrays(GL_LINE_LOOP, 0, vertices_a.size());
		glDrawArrays(GL_POINTS, 0, vertices_a.size());

		//triangle draw call
		// Draw the triangles !
		//glDrawElements(
		//	GL_TRIANGLES,      // mode
		//	indices_a.size(),  // count
		//	GL_UNSIGNED_INT,   // type
		//	(void*)0           // element array buffer offset
		//);

		/*
		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			indices_b.size(),  // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
		*/

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		// Swap the screen buffers
		glfwSwapBuffers(window);

		frames++;
		if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - begin).count()/1000000000 >= 1) {
			cout << frames << " FPS" << endl;
			frames = 0;
			begin = std::chrono::steady_clock::now();
		}
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
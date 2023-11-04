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
float camera_scroll_speed = 1.0;
float edge_scroll_region = 50; //in pixels
float mouse_scroll_speed = 4.0;
bool camera_scroll_continuous = true;
bool enable_edge_scroll = true;
bool debug = false;

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

glm::vec3 debug_line = glm::vec3(0, 0, 1);

glm::vec3 debug_a = glm::vec3(0, 0, 0);
glm::vec3 debug_b = glm::vec3(0, 0, 0);


//TODO:
std::vector<glm::vec4> color_array_a;
GLuint VAO, VBO[2], cVBO[2]; //cVBO Color VBO

std::vector<glm::vec3> vertices_a;
std::vector<glm::vec3> vertices_b;

vector<glm::vec3> ConvertToSphere(vector<glm::vec3> vertices, double width) {
	//float height = width / 2;
	float radius_a = (width - 1) / (2 * M_PI);
	float radius_b = (width - 2) / (2 * M_PI);

	vector<glm::vec3> new_vertices;

	for each (glm::vec3 v in vertices)
	{
		float longitude = (v.x / (radius_a * cos(0))) + 0;
		float latitude = ((v.y) / radius_b) + 0;

		//longitude = longitude * M_PI / 180;
		//latitude = latitude * M_PI / 180;

		float x = radius_a * cos(longitude) * sin(latitude);
		float y = radius_a * sin(longitude) * sin(latitude);
		float z = radius_a * cos(latitude);


		new_vertices.push_back(glm::vec3(x, y, z));
		//cout << "x:" << v.x << " y:" << v.y << " z:" << v.z << endl;
	}

	return new_vertices;
}

//TODO: fix
bool CalculatePlaneIntersection(glm::vec3* contact, glm::vec3 rayOrigin, glm::vec3 ray) {
	glm::vec3 normal = glm::vec3(0, 0, 1);

	// get d value
	float d = glm::dot(normal, glm::vec3(100,50,0));

	if (glm::dot(normal, ray) == 0) {
		return false; // No intersection, the line is parallel to the plane
	}

	// Compute the X value for the directed line ray intersecting the plane
	float x = (d - glm::dot(normal, rayOrigin)) / glm::dot(normal, ray);

	// output contact point
	*contact = rayOrigin + normalize(ray) * x; //Make sure your ray vector is normalized
	return true;
}

//TODO: fix
void CalculatePointerIntersection(double xInit, double yInit) {
	// check if cursor intersects object
	cout << "x:" << cam_dir.x << " y:" << cam_dir.y << " z:" << cam_dir.z << endl;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float mouseX = xInit / (width * 0.5f) - 1.0f;
	float mouseY = yInit / (height * 0.5f) - 1.0f;

	glm::mat4 proj = glm::perspective(45.f, 1.f, 0.1f, 1000.f);
	//glm::mat4 view = glm::lookAt(glm::vec3(0.0f), CameraDirection, CameraUpVector);

	glm::mat4 invVP = glm::inverse(proj * view_matrix);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	glm::vec3* contact = new glm::vec3(0,0,0);
	cout << CalculatePlaneIntersection(contact, cam_pos, dir);
	cout << "x:" << contact->x << " y:" << contact->y << " z:" << contact->z << endl;
}

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
		debug = !debug;
	}
}

//www.glfw.org/docs/latest/input_guide.html#input_mouse_button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//sets the initial position of the mouse
		glfwGetCursorPos(window, &xInit, &yInit);
		drag = true;

		CalculatePointerIntersection(xInit, yInit);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		drag = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	cam_pos += glm::vec3(0, 0, (-yoffset)*mouse_scroll_speed);
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
	if (xpos_old == 0) {
		xpos_old = xpos;
		ypos_old = ypos;
	}

	float dx = xpos - xpos_old;
	float dy = ypos - ypos_old;

	if (drag) {
		glm::vec3  cam_right = glm::normalize(glm::cross(cam_dir, cam_up));
		cam_dir += cam_right * (dx / 100.f);
		cam_right = glm::normalize(glm::cross(cam_dir, cam_up));
		cam_dir -= cam_up * (dy / 100.f);
		cam_up = glm::normalize(glm::cross(cam_dir, -cam_right));

		cam_dir = glm::normalize(cam_dir);
	}
	xpos_old = xpos;
	ypos_old = ypos;

	//glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
}
//**** END CALLBACK FUNCTIONS ****//

void Draw(GLuint VAO, GLuint VBO, vector<unsigned int> indices, bool debug) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);

	if (debug) {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawPlanet(GLuint VAO, 
				GLuint VBO, 
				vector<unsigned int> indices, 
				glm::mat4 modl_matrix, 
				GLuint mm_loc, 
				bool debug = false,
				float angle_in_radians = 0, 
				glm::vec3 axis_of_rotation = glm::vec3(0, 0, 1), 
				float scale = 1, 
				glm::vec3 transl = glm::vec3(0, 0, 0)
) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), angle_in_radians, axis_of_rotation); // where x, y, z is axis of rotation (e.g. 0 1 0)
	glm::mat4 translator = glm::translate(glm::mat4(1.0f), transl);
	glm::mat4 scalor = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f * scale, 1 * scale, 1 * scale));
	modl_matrix = translator * rotator * scalor;
	glUniformMatrix4fv(mm_loc, 1, GL_FALSE, glm::value_ptr(modl_matrix));

	Draw(VAO, VBO, indices, debug);
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

	
	//std::vector<glm::vec3> normals;
	//std::vector<glm::vec2> UVs;
	std::vector<unsigned int> indices_a;
	
	//ask map for vertices and indices.
	vertices_a = map->get_map_vertice_data();

	//vertices_a = ConvertToSphere(vertices_a, 200);
	vertices_b = ConvertToSphere(vertices_a, 200);

	indices_a = map->get_map_indice_data();
	color_array_a = map->get_map_color_data();
	
	//defined outside main function now
	//GLuint VAO[2], VBO[4], cVBO[2]; //cVBO Color VBO

	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices_a.size() * sizeof(glm::vec3), &vertices_a.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_a.size() * sizeof(unsigned int), &indices_a.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//GLuint color_VBO;
	glGenBuffers(1, &cVBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, cVBO[0]);

	glBufferData(GL_ARRAY_BUFFER, color_array_a.size() * sizeof(glm::vec4), &color_array_a.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

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
		
		glfwPollEvents();
		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		if (debug) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices_b.size() * sizeof(glm::vec3), &vertices_b.front(), GL_STATIC_DRAW);
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices_a.size() * sizeof(glm::vec3), &vertices_a.front(), GL_STATIC_DRAW);
		}

		DrawPlanet(VAO, VBO[1], indices_a, modl_matrix, mm_loc, debug);
		//Draw(VAO, VBO[1], indices_a); //draw planet map
		
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


#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 2) in vec4 color;

uniform mat4 mm;
uniform mat4 vm;
uniform mat4 pm;

//uniform vec4 inVertexColor;

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = pm * vm * mm * vec4(position.x, position.y, position.z, 1.0);
	vertexColor = color; // set the output variable to a dark-red color
	//vertexColor = glm::vec4(1.0f,0.0f,0.5f,1.0f); // set the output variable to a dark-red color
}
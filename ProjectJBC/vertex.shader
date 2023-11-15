
#version 330 core
  
layout (location = 0) in vec4 position;
layout (location = 2) in vec4 color;

uniform mat4 mm;
uniform mat4 vm;
uniform mat4 pm;

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = pm * vm * mm * position;
	vertexColor = color;
}
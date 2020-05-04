
#version 330 core
  
layout (location = 0) in vec3 position;

uniform mat4 mm;
uniform mat4 vm;
uniform mat4 pm;

void main()
{
    gl_Position = pm * vm * mm * vec4(position.x, position.y, position.z, 1.0);
}
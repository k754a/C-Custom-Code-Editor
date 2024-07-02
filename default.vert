#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform float scale;

void main()
{
//set size and pos
   gl_Position = vec4(aPos.x + aPos.x * scale , aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
//set colors from vertex
   color = aColor;
};
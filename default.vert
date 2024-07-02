#version 330 core

layout (location = 0) in vec3 aPos;

layout (location = 1) in vec3 aColor;

layout (location = 2) in vec2 aTex;



out vec3 color;

out vec2 texCoord;

//size of the triangles
uniform float scalex, scaley;


void main()
{
	
	gl_Position = vec4(aPos.x + aPos.x * scalex, aPos.y + aPos.y * scaley, aPos.z + aPos.z, 1.0);
	// set colors
	color = aColor;
	// set size and pos
	texCoord = aTex;
}
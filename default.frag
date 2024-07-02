#version 330 core
out vec4 FragColor;

in vec3 color;
void main()
{
//just get colors from default
   FragColor = vec4(color, 1.0f);
};
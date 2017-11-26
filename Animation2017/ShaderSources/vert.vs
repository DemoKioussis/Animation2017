#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 norm;

layout (location = 3) in mat4 transform;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out vec3 sendColor;
flat out vec3 sendNorm;

out vec3 TexCoords;

void main()
{
//	gl_Position = projection*view * model * vec4(pos, 1.0f);
	gl_Position = projection*view * transform * vec4(pos, 1.0f);
	TexCoords=pos;
	sendColor = color;
	sendNorm= (transform*vec4(-norm,0)).xyz;

}
#version 330 core
layout (location = 0) in vec3 aPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out float dist;

void main()
{
	//gl_Position = model*vec4(aPos, 1.0f);
		gl_Position = projection*view * model * vec4(aPos, 1.0f);
	//gl_Position =  vec4(aPos, 1.0f);
	dist = gl_Position.z;
}
#version 330 core


flat in vec3 sendNorm;
in vec3 sendColor;
out vec4 FragColor;

uniform brightness;
void main()
{
	
	FragColor = vec4(sendColor*brightness,1);
}   



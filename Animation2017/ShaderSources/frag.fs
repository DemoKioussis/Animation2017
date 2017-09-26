#version 330 core

flat in vec3 sendNorm;
in vec3 sendColor;
uniform vec3 lightDirection; 

out vec4 FragColor;


void main()
{
	
	

	vec3 light = normalize(vec3(1,1,0));
	float prod = dot(normalize(sendNorm),light);
	vec3 result = vec3(prod,prod,prod)*0.5;
   FragColor = vec4((0.5,0.5,0.5)+result,1);
}   



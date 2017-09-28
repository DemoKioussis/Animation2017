#version 330 core


flat in vec3 sendNorm;
flat in vec3 sendColor;

//Light
uniform vec3 lightDirection; 
uniform vec3 lightColor;

uniform vec3 ambientLight;

out vec4 FragColor;


void main()
{
	

	vec3 _lightDirection = normalize(lightDirection);
	float prod = max(dot(normalize(sendNorm),_lightDirection),0);
	
	vec3 result = vec3(prod,prod,prod)*0.6;
	
	
	vec3 mainColor = (sendColor*ambientLight)*0.4;
	
	
	FragColor = vec4(mainColor + result,1);
}   



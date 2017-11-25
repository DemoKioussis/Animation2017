#version 330 core


flat in vec3 sendNorm;
flat in vec3 sendColor;

//Light
uniform vec3 lightDirection; 
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform int isRenderSkybox;

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	
	if(isRenderSkybox==0){
		vec3 _lightDirection = normalize(lightDirection);
		float prod = max(dot(normalize(sendNorm),_lightDirection),0);
	
		vec3 result = vec3(prod,prod,prod)*0.6;
		
		vec3 lightDirection2=vec3(-lightDirection.x,0,-lightDirection.z);
		vec3 _lightDirection2 = normalize(lightDirection2);
		float prod2 = max(dot(normalize(sendNorm),_lightDirection2),0);
	
		vec3 result2 = vec3(prod2,prod2,prod2)*0.3;
	
		vec3 mainColor = (sendColor*ambientLight)*0.5;
	
	
		FragColor = vec4(mainColor + result+result2,1);
	}
	else{
		FragColor = texture(skybox, TexCoords);
	}
}   



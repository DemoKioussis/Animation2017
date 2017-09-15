#version 330 core
out vec4 FragColor;
in float dist;
uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
	float temp = 0.5*dist / 5;
	  //  FragColor = vec4(0.5+dist,0.5+dist,0.5+dist,1);
      FragColor = vec4(temp,temp,temp,1);

}   
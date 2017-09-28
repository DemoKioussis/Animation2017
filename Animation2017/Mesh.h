#pragma once
#include "Shader.h"
#include <vector>
#include <glm\glm.hpp>

#include "Vertex.h"
#include "Renderable.h"
#include "Component.h"
using namespace std;


class Mesh : public Renderable, public Component{
private:
	Shader* shader;

	vector<GLfloat>* vertices;
	vector<GLuint>* indices;


	GLuint VBO, VAO, EBO;

public:

	Mesh();
	~Mesh();

	void setVerticies(vector<GLfloat>& p, vector<GLfloat>& c, vector<GLfloat>& n);
	void setVerticies(vector<GLfloat>* v);
	void setIndices(vector<GLuint>* i);

	void draw();

	void setShader(Shader* s);


	//public for now
	glm::mat4 transform;


};
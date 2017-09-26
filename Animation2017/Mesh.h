#pragma once
#include "Shader.h"
#include <vector>
#include <glm\glm.hpp>
#include "Vertex.h"
using namespace std;


class Mesh {
private:
	Shader* shader;

	vector<GLfloat>* vertices;
	vector<GLuint>* indices;



	glm::mat4x4 transform;

	GLuint VBO, VAO, EBO;

public:

	Mesh();
	~Mesh();

	void setVerticies(vector<Vertex>* v);
	void setVerticies(vector<GLfloat>* v);
	void setVerticies(GLfloat* v);

	void setIndices(vector<GLuint>* i);
	void setIndices(GLuint* i);

	void draw();

	void setShader(Shader* s);


};
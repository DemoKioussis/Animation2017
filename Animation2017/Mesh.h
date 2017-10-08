#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "Renderable.h"
#include <glad\glad.h>
using namespace std;
class Mesh : public Renderable{
private:
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
};
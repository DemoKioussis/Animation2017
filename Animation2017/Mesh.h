#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "Renderable.h"
#include <glad\glad.h>

#define USE_INSTANCING

using namespace std;
class RenderEngine;
class Mesh {
	friend class RenderEngine;
private:
	vector<GLfloat>* vertices;
	vector<GLuint>* indices;
	GLuint VBO, VAO, EBO, transformBuffer;
	int numPrims;
public:

	Mesh();
	~Mesh();

	void setVerticies(vector<GLfloat>& p, vector<GLfloat>& c, vector<GLfloat>& n);
	void setVerticies(vector<GLfloat>* v);
	void setIndices(vector<GLuint>* i);
	void setBufferData(vector<glm::mat4> & data);
	vector<GLfloat>* getVerticies();
	void draw();
};
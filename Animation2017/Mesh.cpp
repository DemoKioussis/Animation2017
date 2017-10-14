#include "Mesh.h"
#include "Renderable.h"


/* The mesh class will hold data about rendering an object. Ideally physical properties would be stored in a 
	rigid body class or something (think unity if you've used it). 
*/
Mesh::Mesh() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	vertices = new std::vector<GLfloat>(0);
	indices = new std::vector<GLuint>(0);

}
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	delete(vertices);
	delete(indices);
}
#pragma region Setters


void Mesh::setVerticies(std::vector<GLfloat>& p, std::vector<GLfloat>& c, std::vector<GLfloat>& n) {
	assert(p.size() == c.size());
	assert(p.size() == n.size());
	assert(p.size() % 3 == 0);
	std::vector<GLfloat> *v = new std::vector<GLfloat>(0);
	for (int i = 0; i < p.size();i+=3) {
		int index0 = i;
		int index1 = i + 1;
		int index2 = i + 2;

		v->push_back(p[index0]);
		v->push_back(p[index1]);
		v->push_back(p[index2]);

		v->push_back(c[index0]);
		v->push_back(c[index1]);
		v->push_back(c[index2]);

		v->push_back(n[index0]);
		v->push_back(n[index1]);
		v->push_back(n[index2]);
	}
	setVerticies(v);
}
void Mesh::setVerticies(std::vector<GLfloat>* v) {

	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	vertices = v;
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), &vertices->front(), GL_STATIC_DRAW);

	//	glBindVertexArray(0);

}
void Mesh::setIndices(std::vector<GLuint>* i) {
	glBindVertexArray(VAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), NULL, GL_STATIC_DRAW);
	indices = i;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), &indices->front(), GL_STATIC_DRAW);

	//	glBindVertexArray(0);
}

#pragma endregion

#pragma region Getters

vector<GLfloat>* Mesh::getVerticies()
{
	return vertices;
}

#pragma endregion

void Mesh::draw() {
		glBindVertexArray(VAO); 
		glDrawElements(GL_TRIANGLES, sizeof(GLuint)*indices->size(), GL_UNSIGNED_INT, 0);


}
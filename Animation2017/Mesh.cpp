#include "Mesh.h"


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


void Mesh::setVerticies(std::vector<Vertex> *v) {
	std::vector<GLfloat> verts(0);
	for (int i = 0; i < v->size();i++) {
		
		for (int j = 0; j < 3;j++) {
			verts.push_back(v->at(i).position[j]);
		}
		for (int j = 0; j < 3;j++) {
			verts.push_back(v->at(i).color[j]);
		}
		for (int j = 0; j < 3;j++) {
			verts.push_back(v->at(i).normal[j]);
		}
	}
	setVerticies(&verts);
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



void Mesh::setVerticies(GLfloat* v) {
	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	//	vertices = v;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//	glBindVertexArray(0);

}

void Mesh::setIndices(GLuint* i) {
	glBindVertexArray(VAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);
	//	indices = i;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//	glBindVertexArray(0);
}

void Mesh::setShader(Shader* s) {
	shader = s;
}

void Mesh::draw() {

	// draw our first triangle
	shader->use();
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, sizeof(GLuint)*indices->size(), GL_UNSIGNED_INT, 0);


}
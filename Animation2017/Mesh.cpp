#include "Mesh.h"
#include "Renderable.h"
#include <iostream>
/* The mesh class will hold data about rendering an object. Ideally physical properties would be stored in a 
	rigid body class or something (think unity if you've used it). 
*/
Mesh::Mesh() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &transformBuffer);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // norm
	glEnableVertexAttribArray(2);



	vertices = new std::vector<GLfloat>(0);
	indices = new std::vector<GLuint>(0);

}

Mesh::Mesh(char* objName, MeshType type) : meshType(type)
{

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &transformBuffer);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // norm
	glEnableVertexAttribArray(2);



	vertices = new std::vector<GLfloat>(0);
	indices = new std::vector<GLuint>(0);
	makeMesh(objName);
}
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

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
void Mesh::setVerticiesStaticColour(std::vector<GLfloat>& p, std::vector<GLfloat>& c, std::vector<GLfloat>& n) {
	assert(p.size() == n.size());
	assert(p.size() % 3 == 0);
	std::vector<GLfloat> *v = new std::vector<GLfloat>(0);
	for (int i = 0; i < p.size(); i += 3) {
		int index0 = i;
		int index1 = i + 1;
		int index2 = i + 2;

		v->push_back(p[index0]);
		v->push_back(p[index1]);
		v->push_back(p[index2]);

		v->push_back(c[0]);
		v->push_back(c[1]);
		v->push_back(c[2]);

		v->push_back(n[index0]);
		v->push_back(n[index1]);
		v->push_back(n[index2]);

		}
	
	setVerticies(v);
}

//void Mesh::setVerticiesStaticColour(std::vector<GLfloat>& p, std::vector<GLfloat>& c) {
//	//assert(p.size() == n.size());
//	std::cout << "size: " << p.size() << endl;
//	assert(p.size() % 3 == 0);
//	std::vector<GLfloat> *v = new std::vector<GLfloat>(0);
//	for (int i = 0; i < p.size(); i += 6) {
//		int index0 = i;
//		int index1 = i + 1;
//		int index2 = i + 2;
//		int index3 = i + 3;
//		int index4 = i + 4;
//		int index5 = i + 5;
//		v->push_back(p[index0]);
//		v->push_back(p[index1]);
//		v->push_back(p[index2]);
//
//		v->push_back(c[0]);
//		v->push_back(c[1]);
//		v->push_back(c[2]);
//
//		//v->push_back(p[index3]);
//		//v->push_back(p[index4]);
//		//v->push_back(p[index5]);
//
//		//v.push_back(p[index0]);
//		//v.push_back(p[index1]);
//		//v.push_back(p[index2]);
//
//		//v.push_back(c[0]);
//		//v.push_back(c[1]);
//		//v.push_back(c[2]);
//
//		//v.push_back(n[index0]);
//		//v.push_back(n[index1]);
//		//v.push_back(n[index2]);
//
//
//		if (i == 0) {
//			vector<GLfloat> &vr = *v;
//			//GLfloat te[] = v->front();
//			for (int j = 0; j < vr.size(); j++) {
//				std::cout << vr[j] << std::endl;
//			}
//		}
//	}
//	cout << "size after "<<v->size() << endl;
//	setVerticies(v);
//
//
//}
void Mesh::setVerticies(std::vector<GLfloat>* v) {

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	vertices = v;
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), &vertices->front(), GL_STATIC_DRAW);

	//	glBindVertexArray(0);

}
void Mesh::setIndices(std::vector<GLuint>* i) {

	//glBindVertexArray(VAO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), NULL, GL_STATIC_DRAW);

	indices = i;
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), &indices->front(), GL_STATIC_DRAW);

	//	glBindVertexArray(0);
}

void Mesh::setBufferData(vector<glm::mat4> & data) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,transformBuffer);
	if (numPrims < data.size()) {
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), &data[0],GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));


		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
	}
	else
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(glm::mat4), &data[0]);


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
		//glDrawArrays(GL_TRIANGLES, 0, vertices->size());
		//glDrawElements(GL_TRIANGLES, sizeof(GLuint)*indices->size(), GL_UNSIGNED_INT, 0);
#ifndef USE_INSTANCING
	//glDrawElements(GL_TRIANGLES, sizeof(GLuint)*indices->size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices->size());
#else
	//	glDrawArrays(GL_TRIANGLES, 0, vertices->size());
	glDrawArraysInstanced(GL_TRIANGLES,  0, sizeof(GLfloat)*vertices->size(), numPrims);

#endif

}


void Mesh::makeMesh(char* objName) {


	//Mesh*m = new Mesh();
	vector<GLfloat> positionTest;
	vector<GLfloat> colourTest = { 0.50f,0.5,0.5 };
	vector<GLfloat> normalsTest;
	vector<GLuint> indicesTest;
	char pathfile[100];
	strcpy_s(pathfile, "Models/");
	strcat_s(pathfile, objName);
	loadOBJNoUV(pathfile, positionTest, normalsTest, indicesTest);
	//m->setVerticiesStaticColour(positionTest, colourTest, normalsTest);
	//m->setIndices(&indicesTest);
	setVerticiesStaticColour(positionTest, colourTest, normalsTest);
	setIndices(&indicesTest);

	//m->setVerticies(positions, colors, normals);
	//m->setIndices(inds);
	;
}

MeshType Mesh::getMeshType()
{
	return meshType;
}

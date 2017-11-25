#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "Renderable.h"
#include <glad\glad.h>
#include "objloader.hpp"

#define USE_INSTANCING

enum class MeshType
{
	VERTICES,
	SPHERE,
	SKYBOX
};

enum class MeshShape {
	CUBE,
	CYLINDER,
	ELLIPSOID,
	SPHERE,
	SPHERELR,
	SPHEREHD,
};

using namespace std;
class RenderEngine;
class Mesh {
	friend class RenderEngine;
private:
	vector<GLfloat>* vertices;
	vector<glm::vec3> uniqueVertices;

	vector<GLuint>* indices;
	GLuint VBO, VAO, EBO, transformBuffer;
	int numPrims;
	MeshType meshType;
public:

	Mesh();
	Mesh(char* objName, MeshType type);
	~Mesh();
	void setVerticies(vector<GLfloat>& p, vector<GLfloat>& c, vector<GLfloat>& n);
	void setVerticiesStaticColour(std::vector<GLfloat> &p, std::vector<GLfloat> &c, std::vector<GLfloat> &n);
	void setUniqueVerts(vector<GLfloat>& p);
	//void setVerticiesStaticColour(std::vector<GLfloat>& p, std::vector<GLfloat>& c);
	void setVerticies(vector<GLfloat>* v);
	void setIndices(vector<GLuint>* i);
	void setBufferData(vector<glm::mat4> & data);
	vector<GLfloat>* getVerticies();
	void draw();
	void Mesh::makeMesh(char* objName);
	const vector<glm::vec3>* Mesh::getUniqueVerts();
	MeshType getMeshType();
	static unsigned int  cubemapTexture;

};
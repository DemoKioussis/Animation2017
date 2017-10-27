#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>


bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals,
	std::vector<glm::vec2> & out_uv
);
bool loadOBJNoUV(
	const char * path,
	std::vector<GLfloat> & out_vertices,
	std::vector<GLfloat> &out_normals,
	std::vector<GLuint> &out_indices
);


#endif
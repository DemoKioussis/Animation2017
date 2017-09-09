#pragma once
#include <glad\glad.h>
#include <glm\glm.hpp>


/**
	this class will be what we finially store in our vbo's. Right now im just using position data, 
	however we will need info about normals for collisions and such,
	and UV's for textures if we end up using textures
*/
typedef struct
{
	GLfloat x, y, z;
	GLfloat nx, ny, nz;
	GLfloat u, v;
}
Vertex;

class VertexBuilder {

public:
	Vertex genVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv) {
		Vertex vert;

		setPos(vert, pos);
		setNorm(vert, norm);
		setUV(vert, uv);

		return vert;
	}

	void setPos(Vertex &vert, glm::vec3 pos) {
		vert.x = pos.x;
		vert.y = pos.y;
		vert.z = pos.z;
	}

	void setNorm(Vertex &vert, glm::vec3 norm) {
		vert.nx = norm.x;
		vert.ny = norm.y;
		vert.nz = norm.z;
	}

	void setUV(Vertex &vert, glm::vec2 uv) {
		vert.u = uv.x;
		vert.v = uv.y;
	}

};

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
	
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
}
Vertex;

class VertexBuilder {

public:
	static Vertex genVertex(glm::vec3 pos, glm::vec3 norm, glm::vec3 c) {
		Vertex vert;

		setPos(vert, pos);
		setNorm(vert, norm);
		setColor(vert, c);

		return vert;
	}

	static std::vector<Vertex> makeVertexList(std::vector<float> &positions, std::vector<float> normals, std::vector<float> colors) {
		
		std::vector<Vertex> vertecies(0);

		for (int i = 0; i < positions.size();i++) {
			glm::vec3 pos(positions[i]);
			glm::vec3 norm;
			glm::vec3 col;

			if (normals.size() < i) {
				norm = glm::vec3(0, 0, 0);
			}
			if (colors.size() < i) {
				col = glm::vec3(0, 0, 0);
			}
			vertecies.push_back(genVertex(pos, norm, col));
		}

		return vertecies;
	}
private:
	static void setPos(Vertex &vert, glm::vec3 pos) {
		vert.position = pos;
	}

	static void setNorm(Vertex &vert, glm::vec3 norm) {
		vert.normal = norm;
	}

	static void setColor(Vertex &vert, glm::vec3 c) {
		vert.color = c;

	}

};

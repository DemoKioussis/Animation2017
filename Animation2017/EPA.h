#pragma once

#include "GJK.h"

class GJK;

struct Face
{
	Face(glm::vec3& pa, glm::vec3& pb, glm::vec3& pc) : a(pa), b(pb), c(pc)
	{
		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;

		normal = glm::normalize(glm::cross(ab, ac));		
	}

	const glm::vec3 a;
	const glm::vec3 b;
	const glm::vec3 c;

	glm::vec3 getNormal()
	{
		return normal;
	}

private: 
	glm::vec3 normal;
};

class EPA
{
	GJK& gjk;
	std::vector<Face*> faces;

	float distanceFromFaceToOrigin(Face& face);
	Face* getClosestFaceToOrigin(float& closestFaceDistance);
	glm::vec3 projectionOnNormal(glm::vec3 supportPoint, glm::vec3 normal);
	void extendPolytope(glm::vec3 extendPoint);
	bool canSeeFaceFromPoint(Face& face, glm::vec3& point);
	void addOrRemoveEdge(std::vector<std::pair<glm::vec3, glm::vec3>>& edges, std::pair<glm::vec3, glm::vec3>& edge);
public:
	EPA(GJK& gjkWithCollision);
	glm::vec3 getPenetrationVector();
};
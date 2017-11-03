#pragma once

#include "GJK.h"

class GJK;

struct Face
{
	Face(glm::vec3& pa, glm::vec3& pb, glm::vec3& pc) : a(pa), b(pb), c(pc)
	{

	}

	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;

	glm::vec3 getNormal()
	{
		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;

		return glm::normalize(glm::cross(ab, ac));
	}
};

class EPA
{
	GJK& gjk;
	std::vector<Face> faces;

	float distanceFromFaceToPoint(Face& face, glm::vec3 point);
	Face& getClosestFaceToOrigin(float& closestFaceDistance);
	float projectionOnNormalLength(glm::vec3 supportPoint, glm::vec3 normal);
public:
	EPA(GJK& gjkWithCollision);
	glm::vec3 getPenetrationVector();
};
#pragma once

#include <unordered_set>
#include "RenderEngine.h"
#include "CollisionComponent.h"
#include "Entity.h"
#include "RenderComponent.h"

class GJK
{
	CollisionComponent & c1; 
	CollisionComponent & c2;

	glm::mat4& transform1;
	glm::mat4& transform2;

	glm::mat4 transform1I;
	glm::mat4 transform2I;
	 
	std::vector<int>* indices1;
	vector<GLfloat>* vertices1;

	std::vector<int>* indices2;
	vector<GLfloat>* vertices2;

	// Important GJK variables
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 d;
	int pointsCount = 0;
	unordered_set<int> ignoreIndices1; // Helps to avoid taking the same point twice
	unordered_set<int> ignoreIndices2;

	glm::vec3 support(glm::vec3& directionWC);
	glm::vec3 furthestPointInDirection1(glm::vec3& directionOCnormalized, int& index);
	glm::vec3 furthestPointInDirection2(glm::vec3& directionOCnormalized, int& index);
	bool simplex(glm::vec3& direction);
	bool simplex2(glm::vec3 & direction);
	bool simplex3(glm::vec3 & direction);
	bool simplex4(glm::vec3 & direction);
	bool sameDirection(glm::vec3 & vec1, glm::vec3 & vec2);
	void setPoints(glm::vec3 a);
	void setPoints(glm::vec3 a, glm::vec3 b);
	void setPoints(glm::vec3 a, glm::vec3 b, glm::vec3 c);
	void setPoints(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
	void addPoint(glm::vec3 newA);
public:
	GJK(CollisionComponent & cc1, CollisionComponent & cc2);
	bool areColliding();
	//glm::vec3 gjkSupport(glm::vec3& directionInWorldCoordinates, );
};
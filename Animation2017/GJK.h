#pragma once

#include <unordered_set>
#include "CollisionComponent.h"
#include "Entity.h"
#include "CollisionEngine.h"
#include "EPA.h"

class CollisionData;
class EPA;
class CollisionResult;

class GJK
{
	friend class EPA;

	CollisionComponent & c1; 
	CollisionComponent & c2;

	glm::mat4& transform1;
	glm::mat4& transform2;

	glm::mat4 transform1I;
	glm::mat4 transform2I;	 
	
	const vector<glm::vec3>* vertices1;
	const vector<glm::vec3>* vertices2;

	CollisionData* collisionData1;
	CollisionData* collisionData2;

	MeshType meshType1;
	MeshType meshType2;

	// Important GJK variables
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 d;
	int pointsCount = 0;

	glm::vec3 support(const glm::vec3& directionWC);
	glm::vec3 furthestPointInDirectionVertex(glm::vec3& directionOCnormalized, const std::vector<glm::vec3>& vertices, std::vector<glm::vec4>* pointsForResult = nullptr);  //, std::vector<float>* distancesForResult = nullptr);
	glm::vec3 furthestPointInDirectionSphere(glm::vec3& directionOCnormalized, CollisionComponent& collisionComponent);
	void supportForResult(glm::vec3& penetrationVectorWC, CollisionResult& collisionResult);
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
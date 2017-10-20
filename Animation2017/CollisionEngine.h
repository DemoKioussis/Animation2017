#pragma once
#include "Engine.h"
#include "CollisionComponent.h"
#include <unordered_map>
#include "Mesh.h"
#include <deque>


struct CollisionData
{
	float distanceToFurthestPoint;
	std::vector<int> uniqueVerticesIndices; // Unique vertices indices with the corresponding furthest direction corresponding to that point
};

class CollisionEngine : public Engine
{
	static CollisionEngine* instance;
	std::unordered_map<int, CollisionData> collisionData; // Maps the mesh with its collision data

	void updateCollisionDataForMesh(Mesh* mesh, int meshId);
	
	bool areColliding(CollisionComponent* c1, CollisionComponent* c2);
	bool areSpheresColliding(CollisionComponent* c1, CollisionComponent* c2);
	bool isSphereCollidingWithVertexObject(CollisionComponent* sphere, CollisionComponent* vertexObject);
	bool areCollidingGJK(CollisionComponent* c1, CollisionComponent* c2);
	glm::vec3 gjkSupport(glm::vec3& directionInWorldCoordinates, glm::mat4& modelC1, glm::mat4& inverseModelC1, std::vector<int>& indicesC1, vector<GLfloat>& verticesC1, glm::mat4& modelC2, glm::mat4& inverseModelC2, std::vector<int>& indicesC2, vector<GLfloat>& verticesC2); // GJK support function
	glm::vec3 gjkGetFurthestPointInDirection(glm::mat4& model, glm::vec3& directionInObjectCoordinatesNormalized, std::vector<int>& indices, vector<GLfloat>& vertices);
	bool gjkSimplex(std::deque<glm::vec3>& points, glm::vec3& direction);
	bool simplex2(std::deque<glm::vec3>& points, glm::vec3& direction);
	bool simplex3(std::deque<glm::vec3>& points, glm::vec3& direction);
	bool simplex4(std::deque<glm::vec3>& points, glm::vec3& direction);
	bool sameDirection(glm::vec3& vec1, glm::vec3& vec2);
public:
	static void Initialize();
	static CollisionEngine * getInstance();
	void step();
	void calculateUniqueIndicesAndFurthestDistances();
	
};
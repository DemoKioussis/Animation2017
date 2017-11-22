#pragma once
#include "Engine.h"
#include "CollisionComponent.h"
#include <unordered_map>
#include "Mesh.h"
#include "GJK.h"
#include <mutex>
#include <vector>

class CollisionComponent;
class GJK;

struct CollisionData
{
	//float distanceToFurthestPoint;
	std::vector<int> uniqueVerticesIndices; // Unique vertices indices with the corresponding furthest direction corresponding to that point
};

struct CollisionResult
{
	CollisionComponent* c1;
	CollisionComponent* c2;
	glm::vec3 penetrationVector;

	std::vector<glm::vec4> pointsC1;
	std::vector<float> distancesToPointsFromOriginC1;

	std::vector<glm::vec4> pointsC2;	
	std::vector<float> distancesToPointsFromOriginC2;
};

class CollisionEngine : public Engine
{
	std::vector<Mesh*> meshes;
	float maxRadius;

	static CollisionEngine* instance;
	std::unordered_map<int, CollisionData> collisionData; // Maps the mesh with its collision data
	std::unordered_map<unsigned long long, std::vector<CollisionComponent*>*> vonNeumannGrid;
	std::mutex collisionResultsMutex;
	
	// Dynamic collisions with Von Neumann neighbourhood 
	void createVonNeumannGrid();
	void clearVonNeumannGrid();
	void checkCollisionsVonNeumannGrid();
	unsigned long long hashAndWritePosition(glm::vec4 positionWC, CollisionComponent* cc);
	unsigned long long hashPosition(glm::ivec3 position);
	std::vector<CollisionComponent*>* getAtVonNeumannPosition(glm::ivec3 position);
	bool areCollidingDynamic(CollisionComponent* c1, CollisionComponent* c2);

	// Static collisions using bounding boxes
	void checkStaticCollisions();	
	bool areCollidingStatic(CollisionComponent* s, CollisionComponent* d);
	
	bool areBoundingBoxesColliding(CollisionComponent* box, CollisionComponent* sphere);
	bool isPointInsideBox(CollisionComponent* box, glm::vec4 point);
	bool areSpheresColliding(CollisionComponent* c1, CollisionComponent* c2, bool saveResult);
	bool areCollidingGJK(CollisionComponent* c1, CollisionComponent* c2);
public:
	std::vector<CollisionResult*> collisionResults;
	static void Initialize();
	static void Clear();
	void addMesh(Mesh *_mesh);
	std::vector<Mesh*>& getAllMeshes();
	static CollisionEngine * getInstance();
	void step();
	void calculateUniqueIndices();
	void updateAllBoundingBoxes();
	std::unordered_map<int, CollisionData>& getCollisionData();
	void updateMaxRadius();
	void clearCollisionResults();
	void addCollisionResult(CollisionResult* collisionResult);
};
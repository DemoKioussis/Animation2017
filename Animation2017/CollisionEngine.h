#pragma once
#include "Engine.h"
#include "CollisionComponent.h"
#include <unordered_map>
#include "Mesh.h"
#include "GJK.h"
#include <deque>

class CollisionComponent;

struct CollisionData
{
	float distanceToFurthestPoint;
	std::vector<int> uniqueVerticesIndices; // Unique vertices indices with the corresponding furthest direction corresponding to that point
	//MeshType meshType;
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
public:
	static void Initialize();
	static CollisionEngine * getInstance();
	void step();
	void calculateUniqueIndicesAndFurthestDistances();
	std::unordered_map<int, CollisionData>& getCollisionData();
};
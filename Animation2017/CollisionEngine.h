#pragma once
#include "Engine.h"
#include "CollisionComponent.h"
#include <unordered_map>
#include "Mesh.h"

struct CollisionData
{
	float distanceToFurthestPoint;
	std::vector<int> uniqueVerticesIndices; // Unique vertices indices with the corresponding furthest direction correspondign to that point
};

class CollisionEngine : public Engine
{
	static CollisionEngine* instance;
	std::unordered_map<int, CollisionData> collisionData; // Maps thge mesh with its collision data

	void updateCollisionDataForMesh(Mesh* mesh, int meshId);
public:
	static void Initialize();
	static CollisionEngine * getInstance();
	void step();
	void calculateUniqueIndicesAndFurthestDistances();
	
};
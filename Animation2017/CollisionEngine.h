#pragma once
#include "Engine.h"
#include "CollisionComponent.h"
#include <unordered_map>
#include "Mesh.h"
#include "GJK.h"

class CollisionComponent;
class GJK;

struct CollisionData
{
	//float distanceToFurthestPoint;
	std::vector<int> uniqueVerticesIndices; // Unique vertices indices with the corresponding furthest direction corresponding to that point
};

class CollisionEngine : public Engine
{
	std::vector<Mesh*> meshes;
	

	static CollisionEngine* instance;
	std::unordered_map<int, CollisionData> collisionData; // Maps the mesh with its collision data
	
	bool areColliding(CollisionComponent* c1, CollisionComponent* c2);
	bool areSpheresColliding(CollisionComponent* c1, CollisionComponent* c2);
	bool isSphereCollidingWithVertexObject(CollisionComponent* sphere, CollisionComponent* vertexObject);
	bool areCollidingGJK(CollisionComponent* c1, CollisionComponent* c2);
public:
	static void Initialize();
	void addMesh(Mesh *_mesh);
	std::vector<Mesh*>& getAllMeshes();
	static CollisionEngine * getInstance();
	void step();
	void calculateUniqueIndices();
	void updateAllBoundingBoxesIfStatic();
	std::unordered_map<int, CollisionData>& getCollisionData();
};
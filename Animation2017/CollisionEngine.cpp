#include "CollisionEngine.h"
#include "Entity.h"
#include "GJK.h"

#include <stdexcept>
#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace glm;

CollisionEngine* CollisionEngine::instance = nullptr;

void CollisionEngine::Initialize()
{
	if (instance)
		return;
	CollisionEngine * engine = new CollisionEngine();
	instance = engine;
	getInstance()->maxRadius = 0;
	engine->enable();
}

CollisionEngine * CollisionEngine::getInstance()
{
	return instance;
}

// Calculate uniqe indices
void CollisionEngine::calculateUniqueIndices()
{
	for (int meshId = 0; meshId < meshes.size(); meshId++)
	{
		CollisionData collision;
		vector<GLfloat>& vertices = *meshes[meshId]->getVerticies();
		MeshType meshType = meshes[meshId]->getMeshType();

		// Iterate through all the vertices
		for (int index = 0; index < vertices.size(); index += 9)
		{
			glm::vec3 vertex(vertices[index], vertices[index + 1], vertices[index + 2]);

			// Store the index for the current vertex if there isn't already a vertex with almsot the same position
			bool alreadyExists = false;
			for (int existingIndex : collision.uniqueVerticesIndices)
			{
				vec3 alreadyExisting(vertices[existingIndex], vertices[existingIndex + 1], vertices[existingIndex + 2]);
				float lengthOfDifference = glm::length(vertex - alreadyExisting);
				if (lengthOfDifference < 0.001f)
				{
					// Difference is too small, we consider them as being the same vertex
					alreadyExists = true;
				}
			}
			if (!alreadyExists)
			{
				collision.uniqueVerticesIndices.push_back(index);
			}
		}

		collisionData[meshId] = std::move(collision);
	}
}

void CollisionEngine::updateAllBoundingBoxes()
{
	for (Component* c : targetComponents)
	{		
		CollisionComponent* cc = reinterpret_cast<CollisionComponent*>(c);		
		cc->updateBoundingShapes();
	}
}

void CollisionEngine::addMesh(Mesh * _mesh)
{
	meshes.push_back(_mesh);
}

std::vector<Mesh*>& CollisionEngine::getAllMeshes()
{
	return meshes;
}

void CollisionEngine::step()
{
	if (!isEnabled()) {
		return;
	}

	createVonNeumannGrid();
	checkCollisionsVonNeumannGrid();
	clearVonNeumannGrid();

	// Just for testing the areSpheresColliding() method
	/*bool colliding = areColliding((CollisionComponent*)targetComponents[0], (CollisionComponent*)targetComponents[1]);
	cout << "Are colliding: " << (colliding ? "true" : "false") << endl;*/
}


void CollisionEngine::createVonNeumannGrid()
{	
	for (Component* c : targetComponents)
	{
		CollisionComponent* cc = reinterpret_cast<CollisionComponent*>(c);
		std::vector<int>& indices = collisionData[cc->getMeshID()].uniqueVerticesIndices;
		vector<GLfloat>& vertices = *meshes[cc->getMeshID()]->getVerticies();

		if (c->entity->isStatic())
		{
			continue;
		}

		vec4 positionWC = c->getTransform() * vec4(0, 0, 0, 1);		

		long long hashed = hashAndWritePosition(positionWC, cc);

		if (vonNeumannGrid.find(hashed) == vonNeumannGrid.end())
		{
			vonNeumannGrid[hashed] = new std::vector<CollisionComponent*>;
		}

		vonNeumannGrid[hashed]->push_back(cc);
	}	
}

void CollisionEngine::clearVonNeumannGrid()
{
	for (auto iterator : vonNeumannGrid)
	{
		delete iterator.second;
	}

	vonNeumannGrid.clear();
}

void CollisionEngine::checkCollisionsVonNeumannGrid()
{
	#pragma omp parallel for
	for (int targetComponentIndex = 0; targetComponentIndex < targetComponents.size(); targetComponentIndex++)
	{
		static const int neigboursCount = 14;
		std::vector<CollisionComponent*>* neigbours[neigboursCount];

		CollisionComponent* cc = reinterpret_cast<CollisionComponent*>(targetComponents[targetComponentIndex]);

		neigbours[0] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(0, 0, 0));
		neigbours[1] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(0, 1, -1));
		neigbours[2] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(0, 1, 0));
		neigbours[3] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(0, 1, 1));
		neigbours[4] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(0, 0, 1));
		neigbours[5] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, -1, -1));
		neigbours[6] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, -1, 0));
		neigbours[7] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, -1, 1));
		neigbours[8] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 0, -1));
		neigbours[9] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 0, 0));
		neigbours[10] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 0, 1));
		neigbours[11] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 1, -1));
		neigbours[12] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 1, 0));
		neigbours[13] = getAtVonNeumannPosition(cc->vonNeumannPosition + ivec3(1, 1, 1));

		for (size_t cellIndex = 0; cellIndex < neigboursCount; cellIndex++)
		{
			if (neigbours[cellIndex] != nullptr)
			{
				std::vector<CollisionComponent*>& neigboursSameCell = *neigbours[cellIndex];
				for (size_t i = 0; i < neigboursSameCell.size(); i++)
				{
					if (cc->uid != neigboursSameCell[i]->uid)
					{
						areColliding(cc, neigboursSameCell[i]);
					}
				}
			}						
		}
	}
}

long long CollisionEngine::hashAndWritePosition(vec4 positionWC, CollisionComponent* cc)
{
	float cellSize = maxRadius * 2.1;

	ivec3 position;
	position.x = floor(positionWC.x / cellSize);
	position.y = floor(positionWC.y / cellSize);
	position.z = floor(positionWC.z / cellSize);

	cc->vonNeumannPosition = position;

	return hashPosition(position);
}

long long CollisionEngine::hashPosition(glm::ivec3 position)
{
	long long x = position.x << 42;
	long long y = position.y << 21;
	long long z = position.z;

	return x | y | z;
}

std::vector<CollisionComponent*>* CollisionEngine::getAtVonNeumannPosition(glm::ivec3 position)
{
	long long hashedPosition = hashPosition(position);
	return vonNeumannGrid.find(hashedPosition) != vonNeumannGrid.end() ? vonNeumannGrid[hashPosition(position)] : nullptr;
}

bool CollisionEngine::areColliding(CollisionComponent * c1, CollisionComponent * c2)
{
	Mesh* m1 = meshes[c1->getMeshID()];
	Mesh* m2 = meshes[c2->getMeshID()];

	if (m1->getMeshType() == MeshType::SPHERE && m2->getMeshType() == MeshType::SPHERE)
	{
		// Sphere collides with sphere
		return areSpheresColliding(c1, c2); 
	}
	else // Any other collisions
	{
		return areCollidingGJK(c1, c2); // For testing
	}
}

bool CollisionEngine::areSpheresColliding(CollisionComponent * c1, CollisionComponent * c2)
{
	mat4& transformMatrix1 = c1->entity->transform;
	mat4& transformMatrix2 = c2->entity->transform;

	// Assuming that the origin of both of the entities is at (0,0,0)
	vec4 origin(0, 0, 0, 1);
	
	// Calculate the positions of the origins of both entities in world coordinates
	vec4 centerInWorldCoordinates1 = transformMatrix1 * origin;
	vec4 centerInWorldCoordinates2 = transformMatrix2 * origin;

	float distanceBetweenOrigins = glm::length(centerInWorldCoordinates2 - centerInWorldCoordinates1);
	float sumOfRadiuses = c1->getBoundingRadius() + c2->getBoundingRadius();

	return sumOfRadiuses > distanceBetweenOrigins;
}

bool CollisionEngine::isSphereCollidingWithVertexObject(CollisionComponent * sphere, CollisionComponent * vertexObject)
{

	mat4& transformMatrixSphere = sphere->entity->transform;
	mat4& transformMatrixVertex = vertexObject->entity->transform;

	// Assuming that the origin of both of the entities is at (0,0,0)
	vec4 origin(0, 0, 0, 1);

	// Calculate the positions of the origins of both entities in world coordinates
	vec4 centerInWorldCoordinatesSphere = transformMatrixSphere * origin;
	vec4 centerInWorldCoordinatesVertex = transformMatrixVertex * origin;

	float sphereRadius = sphere->getBoundingRadius();
		 
	float distanceBetweenOrigins = glm::length(centerInWorldCoordinatesVertex - centerInWorldCoordinatesSphere);
	float sumOfRadiuses = sphereRadius + vertexObject->getBoundingRadius();

	if (sumOfRadiuses <= distanceBetweenOrigins)
	{
		// Bounding spheres are not colliding, so we know that the two objects are not colliding
		return false;
	}

	Mesh* currentMesh = meshes[vertexObject->getMeshID()];

	// Check if any of the vertices are inside the sphere
	std::vector<int>& indices = collisionData[vertexObject->getMeshID()].uniqueVerticesIndices;
	vector<GLfloat>& vertices = *currentMesh->getVerticies();
	for (size_t i = 0; i < indices.size(); i++)
	{
		vec4 vertexObjectCoordiantes(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2] , 1);
		vec4 vertexWorldCoordiantes = transformMatrixVertex * vertexObjectCoordiantes;
		float distanceToVertexFromSphereCenter = glm::length(vertexWorldCoordiantes - centerInWorldCoordinatesSphere);

		if (distanceToVertexFromSphereCenter < sphereRadius)
		{
			// A vetex is inside the sphere, so we know that the two object are colliding
			return true;
		}
	}

	// Check the center of the sphere is inside the vertex object
	//// GJK ??????????????????????????????

	return false;
}

bool CollisionEngine::areCollidingGJK(CollisionComponent * c1, CollisionComponent * c2)
{
	GJK gjk(*c1, *c2);
	return gjk.areColliding();
}

std::unordered_map<int, CollisionData>& CollisionEngine::getCollisionData()
{
	return collisionData;
}

void CollisionEngine::updateMaxRadiusIfBigger(float _maxRadius)
{
	maxRadius = max(_maxRadius, maxRadius);
}

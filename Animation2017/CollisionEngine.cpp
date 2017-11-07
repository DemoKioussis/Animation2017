#include "CollisionEngine.h"
#include "Entity.h"
#include "GJK.h"

#include <stdexcept>
#include <iostream>

using namespace std;
using namespace glm;

CollisionEngine* CollisionEngine::instance = nullptr;

void CollisionEngine::Initialize()
{
	if (instance)
		return;
	CollisionEngine * engine = new CollisionEngine();
	instance = engine;
	engine->enable();
}

CollisionEngine * CollisionEngine::getInstance()
{
	return instance;
}

void CollisionEngine::step()
{	
	if (isEnabled()) {
		// Just for testing the areSpheresColliding() method
		bool colliding = areColliding((CollisionComponent*)targetComponents[0], (CollisionComponent*)targetComponents[1]);
		cout << "Are colliding: " << (colliding ? "true" : "false") << endl;
	}
}

// Calculate uniqe indices
void CollisionEngine::calculateUniqueIndices()
{
	for (int meshId = 0; meshId < meshes.size(); meshId++)
	{
		CollisionData collision;
		vector<GLfloat>& vertices = *meshes[meshId]->getVerticies();
		MeshType meshType = meshes[meshId]->getMeshType();

		if (meshType == MeshType::VERTICES)
		{
			collision.distanceToFurthestPoint = 0;

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

				// Calculate the distance to the furthest point				
				float lengthOfVertex = glm::length(vertex);
				if (lengthOfVertex > collision.distanceToFurthestPoint)
				{
					collision.distanceToFurthestPoint = lengthOfVertex;
				}
			}
		}
		else // Sphere
		{
			glm::vec3 vertex(vertices[0], vertices[1], vertices[2]);
			collision.distanceToFurthestPoint = glm::length(vertex);
		}

		collisionData[meshId] = std::move(collision);
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
	float sumOfRadiuses = collisionData[c1->getMeshID()].distanceToFurthestPoint + collisionData[c2->getMeshID()].distanceToFurthestPoint;

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

	float sphereRadius = collisionData[sphere->getMeshID()].distanceToFurthestPoint;
		 
	float distanceBetweenOrigins = glm::length(centerInWorldCoordinatesVertex - centerInWorldCoordinatesSphere);
	float sumOfRadiuses = sphereRadius + collisionData[vertexObject->getMeshID()].distanceToFurthestPoint;

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

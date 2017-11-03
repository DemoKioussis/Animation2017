#include "CollisionEngine.h"
#include "RenderComponent.h"
#include "RenderEngine.h"
#include "Entity.h"
#include "GJK.h"

#include <stdexcept>
#include <algorithm>

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
		//areColliding((CollisionComponent*)targetComponents[0], (CollisionComponent*)targetComponents[1]);
	}
}

void CollisionEngine::calculateUniqueIndicesAndFurthestDistances()
{
	std::vector<Mesh*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();

	for (int i = 0; i < targetComponents.size(); i++)
	{
		// Get the required components for the current entity
		CollisionComponent* collisionComponent = (CollisionComponent*)targetComponents[i];
		
		RenderComponent* renderComponent = (RenderComponent*)collisionComponent->entity->getComponent(RENDER_COMPONENT); // Get the render component	
		
		// Obtain the current mesh
		int currentMeshId = renderComponent->getMeshID();

		if (collisionData.find(currentMeshId) != collisionData.end())
		{
			continue; // Collision info already exists in this mesh, so we skip it
		}

		Mesh* currentMesh = allRenderables[currentMeshId];

		updateCollisionDataForMesh(currentMesh, currentMeshId);
	}
}

// Update the collision data for the current mesh
void CollisionEngine::updateCollisionDataForMesh(Mesh * mesh, int meshId)
{
	CollisionData collision;
	vector<GLfloat>& vertices = *mesh->getVerticies();
	MeshType meshType = mesh->getMeshType();

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

	collisionData[meshId] = std::move(collision);
}

bool CollisionEngine::areColliding(CollisionComponent * c1, CollisionComponent * c2)
{
	std::vector<Mesh*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();
	RenderComponent* renderComponent1 = (RenderComponent*)c1->entity->getComponent(RENDER_COMPONENT);
	RenderComponent* renderComponent2 = (RenderComponent*)c2->entity->getComponent(RENDER_COMPONENT);
	int m1Id = renderComponent1->getMeshID();
	int m2Id = renderComponent2->getMeshID();
	Mesh* m1 = allRenderables[m1Id];
	Mesh* m2 = allRenderables[m2Id];

	if (m1->getMeshType() == MeshType::SPHERE && m2->getMeshType() == MeshType::SPHERE)
	{
		// Sphere collides with sphere
		return areSpheresColliding(c1, c2); 
	}
	//else if (c1->colliderType == ColliderType::SPHERE && c2->colliderType == ColliderType::VERTICES)
	//{
	//	// Sphere collides with vertex object
	//	return isSphereCollidingWithVertexObject(c1, c2);
	//}
	//else if (c1->colliderType == ColliderType::VERTICES && c2->colliderType == ColliderType::SPHERE)
	//{
	//	// Vertex object collides with sphere
	//	return isSphereCollidingWithVertexObject(c2, c1); 
	//}
	else //if (c1->colliderType == ColliderType::VERTICES && c2->colliderType == ColliderType::VERTICES)
	{
		//Vertex object collides with vertex object
		//return areSpheresColliding(c1, c2) ? areCollidingGJK(c1, c2) : false;

		return areCollidingGJK(c1, c2); // For testing
	}
}

bool CollisionEngine::areSpheresColliding(CollisionComponent * c1, CollisionComponent * c2)
{
	RenderComponent* r1 = (RenderComponent*)c1->entity->getComponent(RENDER_COMPONENT);
	RenderComponent* r2 = (RenderComponent*)c2->entity->getComponent(RENDER_COMPONENT);

	mat4& transformMatrix1 = c1->entity->transform;
	mat4& transformMatrix2 = c2->entity->transform;

	// Assuming that the origin of both of the entities is at (0,0,0)
	vec4 origin(0, 0, 0, 1);
	
	// Calculate the positions of the origins of both entities in world coordinates
	vec4 centerInWorldCoordinates1 = transformMatrix1 * origin;
	vec4 centerInWorldCoordinates2 = transformMatrix2 * origin;

	float distanceBetweenOrigins = glm::length(centerInWorldCoordinates2 - centerInWorldCoordinates1);
	float sumOfRadiuses = collisionData[r1->getMeshID()].distanceToFurthestPoint + collisionData[r2->getMeshID()].distanceToFurthestPoint;

	return sumOfRadiuses > distanceBetweenOrigins;
}

bool CollisionEngine::isSphereCollidingWithVertexObject(CollisionComponent * sphere, CollisionComponent * vertexObject)
{
	RenderComponent* sphereRender = (RenderComponent*)sphere->entity->getComponent(RENDER_COMPONENT);
	RenderComponent* vertexObjectRender = (RenderComponent*)vertexObject->entity->getComponent(RENDER_COMPONENT);

	mat4& transformMatrixSphere = sphere->entity->transform;
	mat4& transformMatrixVertex = vertexObject->entity->transform;

	// Assuming that the origin of both of the entities is at (0,0,0)
	vec4 origin(0, 0, 0, 1);

	// Calculate the positions of the origins of both entities in world coordinates
	vec4 centerInWorldCoordinatesSphere = transformMatrixSphere * origin;
	vec4 centerInWorldCoordinatesVertex = transformMatrixVertex * origin;

	float sphereRadius = collisionData[sphereRender->getMeshID()].distanceToFurthestPoint;
		 
	float distanceBetweenOrigins = glm::length(centerInWorldCoordinatesVertex - centerInWorldCoordinatesSphere);
	float sumOfRadiuses = sphereRadius + collisionData[vertexObjectRender->getMeshID()].distanceToFurthestPoint;

	if (sumOfRadiuses <= distanceBetweenOrigins)
	{
		// Bounding spheres are not colliding, so we know that the two objects are not colliding
		return false;
	}

	std::vector<Mesh*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();
	Mesh* currentMesh = dynamic_cast<Mesh*>(allRenderables[vertexObjectRender->getMeshID()]);

	if (currentMesh == nullptr)
	{
		throw runtime_error("An entity having a collider component has a render component that doesn't countain a Mesh");
	}

	// Check if any of the vertices are inside the sphere
	std::vector<int>& indices = collisionData[vertexObjectRender->getMeshID()].uniqueVerticesIndices;
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

#include "CollisionEngine.h"
#include "RenderComponent.h"
#include "RenderEngine.h"
#include "Entity.h"

#include <stdexcept>

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
	
}

void CollisionEngine::calculateUniqueIndicesAndFurthestDistances()
{
	std::vector<Renderable*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();

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

		Mesh* currentMesh = dynamic_cast<Mesh*>(allRenderables[currentMeshId]);

		if (currentMesh == nullptr)
		{
			throw runtime_error("An entity having a collider component has a render component that doesn't countain a Mesh");
		}

		updateCollisionDataForMesh(currentMesh, currentMeshId);
	}
}

// Update the collision data for the current mesh
void CollisionEngine::updateCollisionDataForMesh(Mesh * mesh, int meshId)
{
	CollisionData collision;
	vector<GLfloat>& vertices = *mesh->getVerticies();

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


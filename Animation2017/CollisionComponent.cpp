#include "CollisionComponent.h"
#include "CollisionEngine.h"

using namespace std;
using namespace glm;

class CollisionData;
class CollisionEngine;

int CollisionComponent::uidCounter = 0;

void CollisionComponent::updateBoundingShapes()
{
	if (updateHasBeenCalledOnce)
	{
		isNotPureSphere = true;
	}

	std::vector<Mesh*>& meshes = CollisionEngine::getInstance()->getAllMeshes();
	Mesh* mesh = meshes[getMeshID()];	
	const vector<vec3>& vertices = *mesh->getUniqueVerts();

	if (mesh->getMeshType() == MeshType::VERTICES || (mesh->getMeshType() == MeshType::SPHERE && isNotPureSphere))
	{		
		mat4& transform = getUpdatedTransform();

		vec3 biggest(0, 0, 0);
		vec3 smallest(0, 0, 0);

		for (size_t index = 0; index < vertices.size(); index++)
		{
			vec4 vertexOC(vertices[index], 0);			

			vec4 vertexWC = transform * vertexOC;

			biggest.x = max(vertexWC.x, biggest.x);
			biggest.y = max(vertexWC.y, biggest.y);
			biggest.z = max(vertexWC.z, biggest.z);

			smallest.x = min(vertexWC.x, smallest.x);
			smallest.y = min(vertexWC.y, smallest.y);
			smallest.z = min(vertexWC.z, smallest.z);
		}

		boundingBox.x = max(abs(smallest.x), biggest.x);
		boundingBox.y = max(abs(smallest.y), biggest.y);
		boundingBox.z = max(abs(smallest.z), biggest.z);

		boundingRadius = glm::length(boundingBox);
	}
	else // MeshType is SPHERE
	{
		const glm::vec3& vertex = vertices[0];
		boundingRadius = glm::length(vertex);
		boundingBox = vec3(boundingRadius, boundingRadius, boundingRadius);
	}

	if (!entity->isStatic())
	{
		CollisionEngine::getInstance()->updateMaxRadius();
	}

	updateHasBeenCalledOnce = true;
}
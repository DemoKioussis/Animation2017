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
	vector<GLfloat>& vertices = *mesh->getVerticies();

	if (mesh->getMeshType() == MeshType::VERTICES || (mesh->getMeshType() == MeshType::SPHERE && isNotPureSphere))
	{
		CollisionData& collisionData = CollisionEngine::getInstance()->getCollisionData()[getMeshID()];
		std::vector<int>& indices = collisionData.uniqueVerticesIndices;		
		mat4& transform = getTransform();

		vec3 biggest(0, 0, 0);
		vec3 smallest(0, 0, 0);

		for (size_t index = 0; index < indices.size(); index++)
		{
			vec4 vertexOC(vertices[index], vertices[index + 1], vertices[index + 2], 0);			

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

		boundingRadius = max(boundingBox.x, max(boundingBox.y, boundingBox.z));
	}
	else // MeshType is SPHERE
	{
		glm::vec3 vertex(vertices[0], vertices[1], vertices[2]);
		boundingRadius = glm::length(vertex);
		boundingBox = vec3(boundingRadius, boundingRadius, boundingRadius);
	}

	if (!entity->isStatic())
	{
		CollisionEngine::getInstance()->updateMaxRadiusIfBigger(boundingRadius);
	}

	updateHasBeenCalledOnce = true;
}
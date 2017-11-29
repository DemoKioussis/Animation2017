#include "CollisionComponent.h"
#include "CollisionEngine.h"

using namespace std;
using namespace glm;

class CollisionData;
class CollisionEngine;

int CollisionComponent::uidCounter = 0;

void CollisionComponent::updateBoundingShapes()
{
	std::vector<Mesh*>& meshes = CollisionEngine::getInstance()->getAllMeshes();
	Mesh* mesh = meshes[getMeshID()];	
	const vector<vec3>& vertices = *mesh->getUniqueVerts();
	mat4& transform = getTransform();

	if (mesh->getMeshType() == MeshType::VERTICES)
	{
		vec3 biggest(0, 0, 0);
		vec3 smallest(0, 0, 0);

		for (size_t index = 0; index < vertices.size(); index++)
		{
			vec4 vertexOC(vertices[index], 0);			

			vec4 vertexLC = transform * vertexOC;

			biggest.x = max(vertexLC.x, biggest.x);
			biggest.y = max(vertexLC.y, biggest.y);
			biggest.z = max(vertexLC.z, biggest.z);

			smallest.x = min(vertexLC.x, smallest.x);
			smallest.y = min(vertexLC.y, smallest.y);
			smallest.z = min(vertexLC.z, smallest.z);
		}

		boundingBox.x = max(abs(smallest.x), biggest.x);
		boundingBox.y = max(abs(smallest.y), biggest.y);
		boundingBox.z = max(abs(smallest.z), biggest.z);

		boundingRadius = glm::length(boundingBox);
	}
	else // MeshType is SPHERE
	{
		vec4 vertexOC(vertices[0], 0);
		vec4 vertexLC = transform * vertexOC;
		boundingRadius = glm::length(vec3(vertexLC));
		boundingBox = vec3(boundingRadius, boundingRadius, boundingRadius);
	}

	if (!entity->isStatic())
	{
		CollisionEngine::getInstance()->updateMaxRadius();
	}
}
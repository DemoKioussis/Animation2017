#include "CollisionComponent.h"
#include "CollisionEngine.h"

using namespace std;
using namespace glm;

class CollisionData;
class CollisionEngine;

void CollisionComponent::updateBoundingShape()
{
	std::vector<Mesh*>& meshes = CollisionEngine::getInstance()->getAllMeshes();
	Mesh* mesh = meshes[getMeshID()];
	CollisionData& collisionData = CollisionEngine::getInstance()->getCollisionData()[getMeshID()];
	std::vector<int>& indices = collisionData.uniqueVerticesIndices;
	vector<GLfloat>& vertices = *mesh->getVerticies();
	mat4& transform = getTransform();

	//if (entity->isStatic())
	//{

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

	//}
	//else
	//{
	//	glm::vec3 vertex(vertices[0], vertices[1], vertices[2]);
	//	float radius = glm::length(vertex);

	//	//vec3

	//}
}
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
	// Just for testing the areSpheresColliding() method
	//areColliding((CollisionComponent*)targetComponents[0], (CollisionComponent*)targetComponents[1]);
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

bool CollisionEngine::areColliding(CollisionComponent * c1, CollisionComponent * c2)
{
	if (c1->colliderType == ColliderType::SPHERE && c2->colliderType == ColliderType::SPHERE)
	{
		// Sphere collides with sphere
		return areSpheresColliding(c1, c2); 
	}
	else if (c1->colliderType == ColliderType::SPHERE && c2->colliderType == ColliderType::VERTICES)
	{
		// Sphere collides with vertex object
		return isSphereCollidingWithVertexObject(c1, c2);
	}
	else if (c1->colliderType == ColliderType::VERTICES && c2->colliderType == ColliderType::SPHERE)
	{
		// Vertex object collides with sphere
		return isSphereCollidingWithVertexObject(c2, c1); 
	}
	else if (c1->colliderType == ColliderType::VERTICES && c2->colliderType == ColliderType::VERTICES)
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

	std::vector<Renderable*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();
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
	//using namespace glm::gtx::random;

	// Preparation for GJK
	std::vector<Renderable*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();

	RenderComponent* renderC1 = (RenderComponent*)c1->entity->getComponent(RENDER_COMPONENT);
	RenderComponent* renderC2 = (RenderComponent*)c2->entity->getComponent(RENDER_COMPONENT);

	mat4& transformMatrixC1 = c1->entity->transform;
	mat4& transformMatrixC2 = c2->entity->transform;

	mat4 transformMatrixC1Inverse = glm::inverse(c1->entity->transform);
	mat4 transformMatrixC2Inverse = glm::inverse(c2->entity->transform);	

	Mesh* meshC1 = dynamic_cast<Mesh*>(allRenderables[renderC1->getMeshID()]);
	std::vector<int>& indicesC1 = collisionData[renderC1->getMeshID()].uniqueVerticesIndices;	
	vector<GLfloat>& verticesC1 = *meshC1->getVerticies();

	Mesh* meshC2 = dynamic_cast<Mesh*>(allRenderables[renderC2->getMeshID()]);
	std::vector<int>& indicesC2 = collisionData[renderC2->getMeshID()].uniqueVerticesIndices;
	vector<GLfloat>& verticesC2 = *meshC2->getVerticies();

	// Use a random direction to get the furthest point of that direction
	vec3 furthestPoint = gjkGetFurthestPointInDirection(vec3(1, 1, 1), transformMatrixC1, transformMatrixC1Inverse, indicesC1, verticesC1, transformMatrixC2, transformMatrixC2Inverse, indicesC2, verticesC2);

	return false;
}

glm::vec3 CollisionEngine::gjkGetFurthestPointInDirection(glm::vec3& directionInWorldCoordinates, glm::mat4& modelC1, glm::mat4& inverseModelC1, std::vector<int>& indicesC1, vector<GLfloat>& verticesC1, glm::mat4& modelC2, glm::mat4& inverseModelC2, std::vector<int>& indicesC2, vector<GLfloat>& verticesC2)
{
	vec3 directionInObjectCoordinates = inverseModelC1 * vec4(directionInWorldCoordinates, 0);
	vec3 directionInObjectCoordinatesNormalized = glm::normalize(directionInObjectCoordinates);

	vec3 furthestC1ObjectCoordinates = gjkGetFurthestPointInDirectionSingle(modelC1, directionInObjectCoordinatesNormalized, indicesC1, verticesC1);
	vec3 furthestC2ObjectCoordinates = gjkGetFurthestPointInDirectionSingle(modelC2, -directionInObjectCoordinatesNormalized, indicesC2, verticesC2);

	vec3 furthestC1WorldCoordinates = modelC1 * vec4(furthestC1ObjectCoordinates,1);
	vec3 furthestC2WorldCoordinates = modelC2 * vec4(furthestC2ObjectCoordinates, 1);

	return  furthestC1WorldCoordinates + furthestC2WorldCoordinates; // Could be minus ?
}

glm::vec3 CollisionEngine::gjkGetFurthestPointInDirectionSingle(glm::mat4& model, glm::vec3& directionInObjectCoordinatesNormalized, std::vector<int>& indices, vector<GLfloat>& vertices)
{
	float dotProductMax = -1;
	vec3 biggestVector(0,0,0);

	for (size_t i = 0; i < indices.size(); i++)
	{
		vec3 vertexInObjectCoordiantes(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2]);
		vec3 vertexInObjectCoordiantesNormalized = glm::normalize(vertexInObjectCoordiantes);

		float dotProduct = glm::dot(directionInObjectCoordinatesNormalized, vertexInObjectCoordiantesNormalized);

		if (dotProduct > dotProductMax)
		{
			dotProductMax = dotProduct;
			biggestVector = vertexInObjectCoordiantes;
		}
	}
	vec4 furthestPoint(model * vec4(biggestVector, 0));

	return furthestPoint;
}


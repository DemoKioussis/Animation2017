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
	getInstance()->fakeSphereComponent.setMeshID(-1);
}
void CollisionEngine::Clear() {
	instance->targetComponents.clear();
}
CollisionEngine * CollisionEngine::getInstance()
{
	return instance;
}

void CollisionEngine::updateAllBoundingBoxes()
{
	for (Component* c : targetComponents)
	{		
		CollisionComponent* cc = (CollisionComponent*)c;		
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

	clearCollisionResults();

	createVonNeumannGrid();
	checkCollisionsVonNeumannGrid();
	clearVonNeumannGrid();

	checkStaticCollisions();

	//std::cout << "Number of collisions on this step: " << collisionResults.size() << endl;
}


void CollisionEngine::createVonNeumannGrid()
{	
	for (Component* c : targetComponents)
	{
		CollisionComponent* cc = (CollisionComponent*)c;

		if (c->entity->isStatic())
		{
			continue;
		}

		vec4 positionWC = c->getTransform() * vec4(0, 0, 0, 1);		

		unsigned long long hashed = hashAndWritePosition(positionWC, cc);

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
	for (int dynamicComponentIndex = 0; dynamicComponentIndex < targetComponents.size(); dynamicComponentIndex++)
	{
		CollisionComponent* cc = (CollisionComponent*)targetComponents[dynamicComponentIndex];

		if (cc->entity->isStatic())
		{
			continue;
		}

		static const int neigboursCount = 14;
		std::vector<CollisionComponent*>* neigbours[neigboursCount];

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
					CollisionComponent* cc2 = neigboursSameCell[i];

					// Important to avoid calculating the collision twice for the same pair of objects
					if (cc->vonNeumannPosition != cc2->vonNeumannPosition || cc->uid > cc2->uid) 
					{
						areCollidingDynamic(cc, neigboursSameCell[i]);
					}
				}
			}						
		}
	}
}

unsigned long long CollisionEngine::hashAndWritePosition(vec4 positionWC, CollisionComponent* cc)
{
	float cellSize = maxRadius * 2.1;

	ivec3 position;
	position.x = floor(positionWC.x / cellSize);
	position.y = floor(positionWC.y / cellSize);
	position.z = floor(positionWC.z / cellSize);

	cc->vonNeumannPosition = position;

	return hashPosition(position);
}

unsigned long long CollisionEngine::hashPosition(glm::ivec3 position)
{
	static const unsigned long long mask = 2097151; // 2^21 - 1

	long long x = (*reinterpret_cast<unsigned*>(&position.x) & mask) << 42 ;
	long long y = (*reinterpret_cast<unsigned*>(&position.y) & mask) << 21;
	long long z = (*reinterpret_cast<unsigned*>(&position.z) & mask);

	return x | y | z;
}

std::vector<CollisionComponent*>* CollisionEngine::getAtVonNeumannPosition(glm::ivec3 position)
{
	long long hashedPosition = hashPosition(position);
	return vonNeumannGrid.find(hashedPosition) != vonNeumannGrid.end() ? vonNeumannGrid[hashPosition(position)] : nullptr;
}

void CollisionEngine::checkStaticCollisions()
{
	#pragma omp parallel for
	for (int staticComponentIndex = 0; staticComponentIndex < targetComponents.size(); staticComponentIndex++)
	{
		CollisionComponent* sc = (CollisionComponent*)targetComponents[staticComponentIndex];

		if (!sc->entity->isStatic())
		{
			continue;
		}

		for (int dynamicComponentIndex = 0; dynamicComponentIndex < targetComponents.size(); dynamicComponentIndex++)
		{
			CollisionComponent* dc = (CollisionComponent*)targetComponents[dynamicComponentIndex];

			if (dc->entity->isStatic())
			{
				continue;
			}

			areCollidingStatic(sc, dc);
		}
	}
}

bool CollisionEngine::areCollidingDynamic(CollisionComponent * c1, CollisionComponent * c2)
{
	Mesh* m1 = meshes[c1->getMeshID()];
	Mesh* m2 = meshes[c2->getMeshID()];

	//if (m1->getMeshType() == MeshType::SPHERE && m2->getMeshType() == MeshType::SPHERE && !c1->isNotPureSphere && !c1->isNotPureSphere)
	//{
	//	// Sphere collides with sphere
	//	return areSpheresColliding(c1, c2, true); 
	//}
	//else // Any other collisions
	{
		return areSpheresColliding(c1, c2, false) ? areCollidingGJK(c1, c2) : false; // For testing
	}
}

bool CollisionEngine::areCollidingStatic(CollisionComponent * s, CollisionComponent * d)
{
	return areBoundingBoxesColliding(s,d) ? areCollidingGJK(s, d) : false;
}

bool CollisionEngine::areBoundingBoxesColliding(CollisionComponent* box, CollisionComponent* sphere)
{
	vec3 boxCenter = box->getTransform() * vec4(0, 0, 0, 1);
	vec3 sphereCenter = sphere->getTransform() * vec4(0, 0, 0, 1);
	float radius = sphere->getBoundingRadius();

	vec3 aMin = sphereCenter - vec3(radius, radius, radius);
	vec3 aMax = sphereCenter + vec3(radius, radius, radius);
	
	vec3 bMin = boxCenter - box->getBoundingBox();
	vec3 bMax = boxCenter + box->getBoundingBox();

	// AABB collision detection
	if (!(
		aMax.x < bMin.x ||
		bMax.x < aMin.x ||
		aMax.y < bMin.y ||
		bMax.y < aMin.y ||
		aMax.z < bMin.z ||
		bMax.z < aMin.z
		))
	{
		return true;
	}

	return false;
}

bool CollisionEngine::areSpheresColliding(CollisionComponent * c1, CollisionComponent * c2, bool saveResult)
{
	mat4& transformMatrix1 = c1->entity->transform;
	mat4& transformMatrix2 = c2->entity->transform;

	// Assuming that the origin of both of the entities is at (0,0,0)
	vec4 origin(0, 0, 0, 1);
	
	// Calculate the positions of the origins of both entities in world coordinates
	vec4 centerInWorldCoordinates1 = transformMatrix1 * origin;
	vec4 centerInWorldCoordinates2 = transformMatrix2 * origin;

	vec4 from1To2(centerInWorldCoordinates2 - centerInWorldCoordinates1);
	vec4 from2To1(centerInWorldCoordinates1 - centerInWorldCoordinates2);

	float distanceBetweenOrigins = glm::length(from1To2);
	float sumOfRadiuses = c1->getBoundingRadius() + c2->getBoundingRadius();

	if (sumOfRadiuses > distanceBetweenOrigins)
	{
		if (saveResult)
		{		
			CollisionResult* collisionResult = new CollisionResult;
			collisionResult->c1 = c1;
			collisionResult->c2 = c2;
			collisionResult->penetrationVector = glm::normalize(vec3(from1To2)) * (sumOfRadiuses - distanceBetweenOrigins);
			collisionResult->points1OC.push_back(glm::normalize(from1To2) * c1->getBoundingRadius());
			collisionResult->points2OC.push_back(glm::normalize(from2To1) * c2->getBoundingRadius());
			//collisionResult->distancesToPointsFromOriginC1.push_back(glm::length(vec3(from1To2)));
			//collisionResult->distancesToPointsFromOriginC2.push_back(glm::length(vec3(from1To2)));

			CollisionEngine::getInstance()->addCollisionResult(collisionResult);
		}
		return true;
	}

	return false;
}

bool CollisionEngine::areCollidingGJK(CollisionComponent * c1, CollisionComponent * c2)
{
	GJK gjk(*c1, *c2);
	return gjk.areColliding(true);
}

void CollisionEngine::updateMaxRadius()
{
	float currentMax = 0;

	for (Component* c : targetComponents)
	{
		CollisionComponent* sc = (CollisionComponent*)c;

		if (sc->boundingRadius > currentMax)
		{
			currentMax = sc->boundingRadius;
		}
	}

	maxRadius = currentMax;
}

void CollisionEngine::clearCollisionResults()
{	
	for (size_t i = 0; i < collisionResults.size(); i++)
	{
		delete collisionResults[i];
	}
	collisionResults.clear();
}

void CollisionEngine::addCollisionResult(CollisionResult * collisionResult)
{
	collisionResultsMutex.lock();
	collisionResults.push_back(collisionResult);
	collisionResultsMutex.unlock();
}

CollisionComponent & CollisionEngine::getFakeSphereComponent()
{
	return fakeSphereComponent;
}



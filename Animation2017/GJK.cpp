#include "GJK.h"
#include <iostream>
#include "CollisionComponent.h"

using namespace std;
using namespace glm;

GJK::GJK(CollisionComponent & cc1, CollisionComponent & cc2) : c1(cc1), c2(cc2), transform1(c1.entity->transform), transform2(c2.entity->transform)
{
	std::vector<Mesh*>& meshes = CollisionEngine::getInstance()->getAllMeshes();

	transform1I = glm::inverse(transform1);
	transform2I = glm::inverse(transform2);

	Mesh* mesh1 = meshes[cc1.getMeshID()];
	vertices1 = mesh1->getUniqueVerts();
	meshType1 = mesh1->getMeshType();

	Mesh* mesh2 = meshes[cc2.getMeshID()];
	vertices2 = mesh2->getUniqueVerts();
	meshType2 = mesh2->getMeshType();
}

bool GJK::areColliding()
{
	// Use a random direction to get the furthest point of that direction
	vec3 furthestPoint = support(vec3(1, 1, 1));

	addPoint(furthestPoint);

	vec3 direction = -furthestPoint;

	for (size_t i = 0; i < 25; i++) // Limit of iterations to avoid looping forever in an unlucky case
	{
		vec3 nextPoint = support(direction);

		// No-collision check
		{
			float dotProductPointAndDirection = glm::dot(nextPoint, direction);

			if (dotProductPointAndDirection < 0)
			{
				return false; // No intersection
			}
		}

		addPoint(nextPoint);

		// Collision check
		if (simplex(direction))
		{
			EPA epa(*this);
			vec3 penetrationVector = epa.getPenetrationVector();
			CollisionResult* collisionResult = new CollisionResult;
			collisionResult->c1 = &c1;
			collisionResult->c2 = &c2;
			collisionResult->penetrationVector = penetrationVector;
			supportForResult(penetrationVector, *collisionResult);

			CollisionEngine::getInstance()->addCollisionResult(collisionResult);
			return true;
		}
	}

	return false;
}

glm::vec3 GJK::support(const glm::vec3& directionWC)
{
	// OC means object coordinates, WC means world coordinates
	vec3 directionOC1 = transform1I * vec4(directionWC, 0);
	vec3 directionOC1normalized = glm::normalize(directionOC1);

	vec3 directionOC2 = transform2I * vec4(-directionWC, 0);
	vec3 directionOC2normalized = glm::normalize(directionOC2);

	vec3 furthestOC1 = (meshType1 == MeshType::SPHERE && !c1.getIsNotPureSphere()) ? furthestPointInDirectionSphere(directionOC1normalized, c1) : furthestPointInDirectionVertex(directionOC1normalized, *vertices1);
	vec3 furthestOC2 = (meshType2 == MeshType::SPHERE && !c2.getIsNotPureSphere()) ? furthestPointInDirectionSphere(directionOC2normalized, c2) : furthestPointInDirectionVertex(directionOC2normalized, *vertices2);

	vec3 furthestWC1 = transform1 * vec4(furthestOC1, 1);
	vec3 furthestWC2 = transform2 * vec4(furthestOC2, 1);	

	return  furthestWC1 - furthestWC2;
}

glm::vec3 GJK::furthestPointInDirectionVertex(glm::vec3& directionOCnormalized, const std::vector<glm::vec3>& vertices, std::vector<glm::vec4>* pointsForResult)
{
	float dotProductMax = -1;
	vec3 mostSimilarVectorOC(0, 0, 0);

	for (size_t i = 0; i < vertices.size(); i++)
	{
		const vec3& vertexOC = vertices[i];
		vec3 vertexOCnormalized = glm::normalize(vertexOC);

		float dotProduct = glm::dot(directionOCnormalized, vertexOCnormalized);

		if (dotProduct > dotProductMax)
		{
			dotProductMax = dotProduct;
			mostSimilarVectorOC = vertexOC;
		}
	}

	if (pointsForResult != nullptr)
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			const vec3& vertexOC = vertices[i];
			vec3 vertexOCnormalized = glm::normalize(vertexOC);

			float dotProduct = glm::dot(directionOCnormalized, vertexOCnormalized);

			if (dotProduct > dotProductMax - 0.001 && dotProduct < dotProductMax + 0.001)
			{
				pointsForResult->push_back(vec4(vertexOC,1));
			}
		}
	}

	return mostSimilarVectorOC;
}

glm::vec3 GJK::furthestPointInDirectionSphere(glm::vec3& directionOCnormalized, CollisionComponent& collisionComponent)
{
	return directionOCnormalized * collisionComponent.getBoundingRadius();
}

void GJK::supportForResult(glm::vec3& penetrationVectorWC, CollisionResult& collisionResult)
{
	// OC means object coordinates, WC means world coordinates
	vec3 directionOC1 = transform1I * vec4(penetrationVectorWC, 0);
	vec3 directionOC1normalized = glm::normalize(directionOC1);

	vec3 directionOC2 = transform2I * vec4(-penetrationVectorWC, 0);
	vec3 directionOC2normalized = glm::normalize(directionOC2);

	if (meshType1 == MeshType::SPHERE && !c1.getIsNotPureSphere())
	{
		vec3 pointOC = furthestPointInDirectionSphere(directionOC1normalized, c1);
		collisionResult.pointsC1.push_back(vec4(pointOC,1));
		collisionResult.distancesToPointsFromOriginC1.push_back(glm::length(pointOC));
	}
	else
	{
		furthestPointInDirectionVertex(directionOC1normalized, *vertices1, &collisionResult.pointsC1);
	}

	if (meshType2 == MeshType::SPHERE && !c2.getIsNotPureSphere())
	{
		vec3 pointOC = furthestPointInDirectionSphere(directionOC2normalized, c2);
		collisionResult.pointsC2.push_back(vec4(pointOC, 1));
		collisionResult.distancesToPointsFromOriginC2.push_back(glm::length(pointOC));
	}
	else
	{
		furthestPointInDirectionVertex(directionOC2normalized, *vertices2, &collisionResult.pointsC2);
	}

	vec3 minkowskiDiffPoint = support(penetrationVectorWC);

	vec3 center1 = transform1 * vec4(0, 0, 0, 1);
	for (size_t i = 0; i < collisionResult.pointsC1.size(); i++)
	{
		vec4 point1OC = transform1 * collisionResult.pointsC1[i];
		//collisionResult.pointsC1[i] = point1OC;
		vec3 s = support(vec3(point1OC) - center1);
		collisionResult.distancesToPointsFromOriginC1.push_back(glm::length(s));
	}

	vec3 center2 = transform2 * vec4(0, 0, 0, 1);
	for (size_t i = 0; i < collisionResult.pointsC2.size(); i++)
	{
		vec4 point2OC = transform2 * collisionResult.pointsC2[i];
		//collisionResult.pointsC2[i] = point2OC;
		vec3 s = support(-(vec3(point2OC) - center2));
		collisionResult.distancesToPointsFromOriginC2.push_back(glm::length(s));
	}
}

bool GJK::simplex(vec3& direction)
{
	switch (pointsCount)
	{
	case 2:
		return simplex2(direction);
	case 3:
		return simplex3(direction);
	case 4:
		return simplex4(direction);
	}

	return false;
}

bool GJK::simplex2(glm::vec3 & direction)
{
	vec3 ao = -a;
	vec3 ab = b - a;
	
	if (sameDirection(ab, ao))
	{
		// Origin is between A and B
		// Adding small "noise" to avoid NaN values in the case that ao and ab are completely parallel
		direction = cross(cross(ab + vec3(0,0, ab.x / 100000 + ab.y / 100000 + ab.z / 100000), ao), ab); 
	}
	else
	{
		// Origin is near A
		setPoints(a);
		direction = ao;
	}
	return false;
}

bool GJK::simplex3(glm::vec3 & direction)
{
	vec3 ao = -a;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 abc = cross(ab, ac);	

	if (sameDirection(cross(abc, ac), ao))
	{
		setPoints(a, c);
		return simplex2(direction);		
	}

	if (sameDirection(cross(ab, abc), ao))
	{
		setPoints(a, b);
		return simplex2(direction);
	}

	if (sameDirection(abc, ao))
	{
		direction = abc;
	}
	else
	{
		setPoints(a, c, b);
		direction = -abc;
	}

	return false;
}

bool GJK::simplex4(glm::vec3 & direction)
{
	vec3 ao = -a;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 ad = d - a;
	vec3 abc = cross(ab, ac);
	vec3 adb = cross(ad, ab);
	vec3 acd = cross(ac, ad);

	if (sameDirection(abc, ao))
	{
		setPoints(a, b, c);
		return simplex3(direction);
	}
	if (sameDirection(adb, ao))
	{
		setPoints(a, d, b);
		return simplex3(direction);
	}
	if (sameDirection(acd, ao))
	{
		setPoints(a, c, d);
		return simplex3(direction);
	}

	return true;
}

bool GJK::sameDirection(glm::vec3 & vec1, glm::vec3 & vec2)
{
	return dot(vec1, vec2) > 0;
}

void GJK::setPoints(glm::vec3 pa)
{
	a = pa;
	pointsCount = 1;
}

void GJK::setPoints(glm::vec3 pa, glm::vec3 pb)
{
	a = pa;
	b = pb;
	pointsCount = 2;
}

void GJK::setPoints(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc)
{
	a = pa;
	b = pb;
	c = pc;
	pointsCount = 3;
}

void GJK::setPoints(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pd)
{
	a = pa;
	b = pb;
	c = pc;
	d = pd;
	pointsCount = 4;
}

void GJK::addPoint(glm::vec3 newA)
{
	d = c;
	c = b;
	b = a;
	a = newA;
	pointsCount++;
}
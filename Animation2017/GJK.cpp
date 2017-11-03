#include "GJK.h"
#include <iostream>

using namespace std;
using namespace glm;

GJK::GJK(CollisionComponent & cc1, CollisionComponent & cc2) : c1(cc1), c2(cc2), transform1(c1.entity->transform), transform2(c2.entity->transform)
{
	std::vector<Mesh*>& allRenderables = RenderEngine::getInstance()->getAllRenderReferences();
	
	RenderComponent* render1 = (RenderComponent*)c1.entity->getComponent(RENDER_COMPONENT);
	RenderComponent* render2 = (RenderComponent*)c2.entity->getComponent(RENDER_COMPONENT);

	transform1I = glm::inverse(transform1);
	transform2I = glm::inverse(transform2);

	Mesh* mesh1 = allRenderables[render1->getMeshID()];
	collisionData1 = &CollisionEngine::getInstance()->getCollisionData()[render1->getMeshID()];
	indices1 = &collisionData1->uniqueVerticesIndices;
	vertices1 = mesh1->getVerticies();
	meshType1 = mesh1->getMeshType();

	Mesh* mesh2 = allRenderables[render2->getMeshID()];
	collisionData2 = &CollisionEngine::getInstance()->getCollisionData()[render2->getMeshID()];
	indices2 = &collisionData2->uniqueVerticesIndices;
	vertices2 = mesh2->getVerticies();
	meshType2 = mesh2->getMeshType();
}

bool GJK::areColliding()
{	
	/*vector<vec3> v;
	v.push_back(support(vec3(1, 1, 1)));
	v.push_back(support(vec3(-1, 1, 1)));
	v.push_back(support(vec3(-1, -1, 1)));
	v.push_back(support(vec3(-1, -1, -1)));

	v.push_back(support(vec3(1, -1, 1)));
	v.push_back(support(vec3(1, -1, -1)));
	v.push_back(support(vec3(1, 1, -1)));
	v.push_back(support(vec3(-1, 1, -1)));
	
	cout << "start" << endl;
	for (vec3 t : v)
	{		
		cout << t.x << ", " << t.y << ", " << t.z << endl;
	}*/

	// Use a random direction to get the furthest point of that direction
	vec3 furthestPoint = support(vec3(1, 1, 1));

	addPoint(furthestPoint);

	vec3 direction = -furthestPoint;

	for (size_t i = 0; i < 40; i++) // Limit of iterations to avoid looping forever in an unlucky case
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
			epa.getPenetrationVector();
			return true;
		}
	}

	return false;
}

glm::vec3 GJK::support(glm::vec3& directionWC)
{
	// OC means object coordinates, WC means world coordinates
	vec3 directionOC1 = transform1I * vec4(directionWC, 0);
	vec3 directionOC1normalized = glm::normalize(directionOC1);

	vec3 directionOC2 = transform2I * vec4(-directionWC, 0);
	vec3 directionOC2normalized = glm::normalize(directionOC2);

	vec3 furthestOC1 = (meshType1 == MeshType::VERTICES) ? furthestPointInDirectionVertex(directionOC1normalized, *vertices1, *indices1) : furthestPointInDirectionSphere(directionOC1normalized, collisionData1);
	vec3 furthestOC2 = (meshType2 == MeshType::VERTICES) ? furthestPointInDirectionVertex(directionOC2normalized, *vertices2, *indices2) : furthestPointInDirectionSphere(directionOC2normalized, collisionData2);

	vec3 furthestWC1 = transform1 * vec4(furthestOC1, 1);
	vec3 furthestWC2 = transform2 * vec4(furthestOC2, 1);

	return  furthestWC1 - furthestWC2;
}

glm::vec3 GJK::furthestPointInDirectionVertex(glm::vec3& directionOCnormalized, std::vector<GLfloat>& vertices, std::vector<int>& indices)
{
	float dotProductMax = -1;
	vec3 mostSimilarVectorOC(0, 0, 0);

	for (size_t i = 0; i < indices.size(); i++)
	{
		vec3 vertexOC(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2]);
		vec3 vertexOCnormalized = glm::normalize(vertexOC);

		float dotProduct = glm::dot(directionOCnormalized, vertexOCnormalized);

		if (dotProduct > dotProductMax)
		{
			dotProductMax = dotProduct;
			mostSimilarVectorOC = vertexOC;
		}
	}

	return mostSimilarVectorOC;
}

glm::vec3 GJK::furthestPointInDirectionSphere(glm::vec3& directionOCnormalized, CollisionData* collisionData)
{
	return directionOCnormalized * collisionData->distanceToFurthestPoint;
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
		direction = cross(cross(ab, ao), ab);
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
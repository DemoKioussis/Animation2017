#pragma once
#include "Component.h"
#include "Entity.h"

class CollisionComponent : public Component
{
	friend class CollisionEngine;
	
	int meshID;
	float boundingRadius;
	glm::vec3 boundingBox;
	glm::ivec3 vonNeumannPosition;
	int uid;
	static int uidCounter;

public:
	glm::vec3 fakePosition; // Only used for collision resolution to determine the vertices that are actually colliding

	CollisionComponent() : Component() , boundingBox(0,0,0)
	{
		componentType = COLLISION_COMPONENT;
		uid = uidCounter++;
	}

	const int getMeshID() {
		return meshID;
	};

	void setMeshID(int _i) {
		meshID = _i;
	};

	float getBoundingRadius(float radius)
	{
		boundingRadius = radius;
	}

	float getBoundingRadius()
	{
		return boundingRadius;
	}

	glm::vec3 getBoundingBox()
	{
		return boundingBox;
	}

	int getUid()
	{
		return uid;
	}

	void updateBoundingShapes();
};
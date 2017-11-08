#pragma once
#include "Component.h"
#include "Entity.h"

class CollisionComponent : public Component
{
	friend class CollisionEngine;
	
	int meshID;
	float boundingRadius;
	glm::vec3 boundingBox;

public:
	CollisionComponent() : Component() , boundingBox(0,0,0)
	{
		componentType = COLLISION_COMPONENT;
	}

	const int getMeshID() {
		return meshID;
	};

	void setMeshID(int _i) {
		meshID = _i;
	};

	float getBoundingRadius()
	{
		return boundingRadius;
	}

	glm::vec3 getBoundingBox()
	{
		return boundingBox;
	}

	void updateBoundingShape();
};
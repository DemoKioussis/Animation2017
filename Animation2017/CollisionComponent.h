#pragma once
#include "Component.h"
#include "Entity.h"

class CollisionComponent : public Component
{
	friend class CollisionEngine;
	
	int meshID;
	float boundingRadius;
	glm::vec3 boundingBox;
	bool updateHasBeenCalledOnce;
	bool isNotPureSphere;
	glm::ivec3 vonNeumannPosition;
	int uid;
	static int uidCounter;

public:
	

	CollisionComponent() : Component() , boundingBox(0,0,0), updateHasBeenCalledOnce(false), isNotPureSphere(false)
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

	float getBoundingRadius()
	{
		return boundingRadius;
	}

	glm::vec3 getBoundingBox()
	{
		return boundingBox;
	}

	void updateBoundingShapes();
};
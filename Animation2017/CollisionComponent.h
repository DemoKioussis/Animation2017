#pragma once
#include "Component.h"
#include "CollisionEngine.h"

//enum class ColliderType
//{
//	VERTICES,
//	SPHERE
//};

class CollisionComponent : public Component
{
	friend class CollisionEngine;
	
	int mesh;

public:
	CollisionComponent() : Component() //,colliderType(_colliderType)
	{
		componentType = COLLISION_COMPONENT;
	}

	const int getMeshID() {
		return mesh;
	};

	void setMeshID(int _i) {
		mesh = _i;
	};
};
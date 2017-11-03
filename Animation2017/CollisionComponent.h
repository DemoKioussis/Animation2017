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
	//ColliderType colliderType;
public:
	CollisionComponent() : Component() //,colliderType(_colliderType)
	{
		componentType = COLLISION_COMPONENT;
	}
};
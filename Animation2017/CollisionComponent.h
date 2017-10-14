#pragma once
#include "Component.h"

class CollisionComponent : public Component
{
public:
	CollisionComponent() 
	{
		componentType = COLLISION_COMPONENT;
	}
};
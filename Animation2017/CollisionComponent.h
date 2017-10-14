#pragma once
#include "Component.h"

class CollisionComponent : public Component
{	
	float distanceToFurthestPoint;
public:
	CollisionComponent() 
	{
		componentType = COLLISION_COMPONENT;
	}
};
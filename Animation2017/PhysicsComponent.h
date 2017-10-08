#pragma once
#include "Component.h"

class Entity;
class PhysicsEngine;
class PhysicsComponent : public Component {
	friend class PhysicsEngine;
private:
	glm::vec3 centerOfMass;

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 netForce;

	glm::vec3 momentOfInertia;

	float kineticEnergy; // 1/2 mv^2
	glm::vec3 momentum;		// mv

	float friction;
	float mass;
	glm::vec3 rotationLock;


public:
	PhysicsComponent() {
		componentType = PHYSICS_COMPONENT;
	};
	~PhysicsComponent() {};

};
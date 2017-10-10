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
	float mass;

	glm::vec3 angularVelocity;
	glm::vec3 angularAcceleration;
	glm::vec3 torque;
	glm::mat3 momentOfInertia;


	float kineticEnergy; // 1/2 mv^2
	glm::vec3 momentum;		// mv
	glm::vec3 angularMomentum;

	float friction;
	glm::vec3 rotationLock;

	bool isStatic;
	float gravityMultiplyer;

public:
	PhysicsComponent() {
		componentType = PHYSICS_COMPONENT;
		mass = 1;
		gravityMultiplyer = 1;
	};
	~PhysicsComponent() {};

	void setMass(float _mass) {
		mass = _mass;
	}
	void setMomentOfInertia(glm::mat3 _moment) {
		momentOfInertia = _moment;
	}
	void setGravityMultiply(float _multiplyer) {
		gravityMultiplyer = _multiplyer;
	}
	void setCenterOfMass(glm::vec3 _center) {
		centerOfMass = _center;
	}
	void setRotationLock(bool _x, bool _y, bool _z) {
		float x = _x ? 1.0f : 0.0f;
		float y = _y ? 1.0f : 0.0f;
		float z = _z ? 1.0f : 0.0f;

		rotationLock = glm::vec3(x, y, z);
	}
};
#pragma once
#include "Component.h"
#include "glm\gtx\matrix_operation.hpp"
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
	//	setAngularVelocity(glm::vec3(0), 0);
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
	void setAngularVelocity(glm::vec3 _axis, float _vel) {
		angularVelocity = glm::normalize(_axis) * _vel;
	}
	void addAngularVelocity(glm::vec3 _axis, float _vel) {
		glm::vec4 w =glm::vec4( glm::normalize(_axis) * _vel,0);
		glm::vec4 r = getRotation()*w;
		
		angularVelocity += glm::vec3(r.x, r.y, r.z);
	}
	void addAngularVelocity2(glm::vec3 _axis, float _vel) {
		glm::vec4 w = glm::vec4(glm::normalize(_axis) * _vel, 0);
		glm::vec4 r = glm::inverse(getRotation())*w;

		angularVelocity += glm::vec3(r.x, r.y, r.z);
	}
	void addAngularVelocity3(glm::vec3 _axis, float _vel) {
		glm::vec4 w = glm::vec4(glm::normalize(_axis) * _vel, 0);
		glm::vec4 r = w;

		angularVelocity += glm::vec3(r.x, r.y, r.z);
	}

};
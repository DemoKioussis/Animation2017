#pragma once
#include "Component.h"
#include <iostream>
#include "glm\gtx\matrix_operation.hpp"
class Entity;
class PhysicsEngine;
class PhysicsComponent : public Component {
	friend class PhysicsEngine;
private:

	// shouldnt change dynamically
	glm::vec3 centerOfMass;
	float mass;
	float massInverse;


	glm::vec3 velocity;


	glm::vec3 netForce;
	glm::vec3 netTorque;

	glm::vec3 dP; // dP = netForce
	glm::vec3 dL; // dL = netTorque

	glm::vec3 angularVelocity;
	glm::vec3 angularAcceleration;
	glm::mat4 momentOfInertia;
	glm::mat4 momentOfInertiaInverse;


	float kineticEnergy; // 1/2 mv^2
	glm::vec3 P;		// angular velocity
	glm::vec3 L;		// angular momentum

	float friction;
	float coeffOfRestitution;
	glm::vec3 rotationLock;

	bool isStatic =false;
	float gravityMultiplyer;


public:
	PhysicsComponent() {
		componentType = PHYSICS_COMPONENT;
		mass = 1;
		massInverse = 1;
		gravityMultiplyer = 1;
		velocity = glm::vec3();
		netForce = glm::vec3();
		dP		 = glm::vec3();
		P		 = glm::vec3();
		L		 = glm::vec3();

		angularVelocity = glm::vec3();
		angularAcceleration = glm::vec3();
		netTorque = glm::vec3();
		momentOfInertia = glm::mat4();
		momentOfInertiaInverse = glm::mat4();
		coeffOfRestitution = 0.50f;

	};
	~PhysicsComponent() {};

	void setMass(float _mass) {
		mass = _mass;
		massInverse = 1 / _mass;
		if (isStatic) {
			massInverse = 0;
		}
	}
	void setMomentOfInertia(glm::mat4 _moment) {
		momentOfInertia = _moment;
		momentOfInertiaInverse = glm::inverse(_moment);

		if (isStatic) {
			std::cout << "static moment" << std::endl;
			momentOfInertiaInverse = glm::mat4(0);
			for (int i = 0; i < 4;i++) {
				for (int j = 0; j < 4;j++) {
					momentOfInertiaInverse[i][j] = 0;
				}
			}
		}

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


	void setAngularMomentum(glm::vec3 m) {
		L = m;
	}
	void setMomentum(glm::vec3 m) {
		P = m;
	}
	void setStatic(bool _static) {
		isStatic = _static;
	}
	bool getIsStatic() {
		return isStatic;
	}
	void setCoeffOfRestitution(float cr) {
		coeffOfRestitution = cr;
	}
};
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

	bool isStatic;
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
		coeffOfRestitution = 0.90f;

	};
	~PhysicsComponent() {};

	void setMass(float _mass) {
		mass = _mass;
		massInverse = 1 / _mass;
	}
	void setMomentOfInertia(glm::mat4 _moment) {
		momentOfInertia = _moment;
		momentOfInertiaInverse = glm::inverse(_moment);
		std::cout << "Inetia" << std::endl;

		for (int i = 0; i < 4;i++) {
			for (int j = 0; j < 4;j++) {
				std::cout << momentOfInertia[j][i] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;

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
#pragma once
#include "PhysicsEngine.h"
#include "PhysicsComponent.h"

#include "TimeSystem.h"

#include <glm\gtx\matrix_operation.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
PhysicsEngine* PhysicsEngine::instance = 0;

#pragma region initialization
 void PhysicsEngine::Initialize() {
	 if (instance)
		 return;
	 PhysicsEngine * engine = new PhysicsEngine();
	 instance = engine;
	 instance->gravityEnabled = true;
	 instance->updateTime = 1.0f / 240.0f;
}
 PhysicsEngine * PhysicsEngine::getInstance() {
	 return instance;
 }

 void PhysicsEngine::addAttractor(PhysicsComponent* _component) {
	 attractiveBodies.push_back(_component);
 }

 void PhysicsEngine::setGravity(glm::vec3 _dir, float _mag) {
	 gravity = glm::normalize(_dir)*_mag;
 }

#pragma endregion

#pragma region mainLoop
 void PhysicsEngine::step() {
	 if (TimeSystem::physicsCheck() >= updateTime) {
		 TimeSystem::physicsStep();
		 updatePhysics();
		 applyPhysics();
	 }

 }
void PhysicsEngine::updatePhysics() {
	for (int i = 0; i < targetComponents.size();i++) {
		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		addGravity(component);
		setAcceleration(component);
		setVelocity(component);
	}
}
void PhysicsEngine::applyPhysics() {
	for (int i = 0; i < targetComponents.size();i++) {
		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		energy(component);
		momentum(component);
		rotate(component);
		translate(component);
		reset(component);
	}
}
#pragma endregion

#pragma region calculations
void PhysicsEngine::addForce(PhysicsComponent* _component, glm::vec3 force,glm::vec3 position) {
	_component->netForce += force;
	_component->torque = glm::cross(position, force);
}
void PhysicsEngine::addGravity(PhysicsComponent* _component) {
	_component->netForce += gravity*_component->gravityMultiplyer*_component->mass;
}
void PhysicsEngine::addAttractiveForces(PhysicsComponent* _component) {
	for (int i = 0; i < attractiveBodies.size();i++) {
	}
}

void PhysicsEngine::setAcceleration(PhysicsComponent* _component) {
	_component->acceleration = _component->netForce/_component->mass;

}

void PhysicsEngine::setVelocity(PhysicsComponent* _component) {
	_component->velocity += _component->acceleration*TimeSystem::getPhysicsDeltaTime();
	if (glm::length(_component->velocity) > MAX_SPEED) {
		_component->velocity = glm::normalize(_component->velocity)*MAX_SPEED;
	}	
}

#pragma endregion

#pragma region application
void PhysicsEngine::translate(PhysicsComponent* _component) {
	glm::vec3 translation = _component->velocity*TimeSystem::getPhysicsDeltaTime();
	_component->getTransform() = glm::translate(_component->getTransform(), translation);
}
void PhysicsEngine::rotate(PhysicsComponent* _component) {
	
}
void PhysicsEngine::energy(PhysicsComponent* _component) {
	float speed = glm::length(_component->velocity);
	_component->kineticEnergy = (_component->mass*speed*speed/2.0f);
}
void PhysicsEngine::momentum(PhysicsComponent* _component) {
	_component->momentum = _component->mass*_component->velocity;
}
void PhysicsEngine::reset(PhysicsComponent* _component) {
		_component->acceleration = glm::vec3();
		_component->netForce = glm::vec3();
	

}

#pragma endregion
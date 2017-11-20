#pragma once
#include "PhysicsEngine.h"
#include "PhysicsComponent.h"
#include "Entity.h"
#include "TimeSystem.h"
#include "CollisionEngine.h"

#include <glm\gtx\matrix_operation.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <omp.h>
#include <iostream>
#define stepsPerSecond  240
#define updateTime 1.0f/stepsPerSecond

PhysicsEngine* PhysicsEngine::instance = 0;

#pragma region initialization
 void PhysicsEngine::Initialize() {
	 if (instance)
		 return;
	 PhysicsEngine * engine = new PhysicsEngine();
	 instance = engine;
	 instance->gravityEnabled = true;
	 engine->enable();
}
void PhysicsEngine::Clear() {
	instance->targetComponents.clear();
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
 glm::vec3 PhysicsEngine::getGravity() {
	 return gravity;
 }
#pragma endregion

#pragma region mainLoop
 void PhysicsEngine::step() {
	 if (isEnabled()) {
		 if (TimeSystem::physicsCheck() >= updateTime) {
			 TimeSystem::physicsStep();			 
			 updatePhysics();
			 applyPhysics();
			 CollisionEngine::getInstance()->step(); // Collision detection should only happen at the physical step
		 }
	 }

 }

void PhysicsEngine::updatePhysics() {
	#pragma omp parallel for
	for (int i = 0; i < targetComponents.size();i++) {

		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		addGravity(component);
		setAcceleration(component);
		setMomentum(component);
		setVelocity(component);
	}
}

void PhysicsEngine::applyPhysics() {
	#pragma omp parallel for
	for (int i = 0; i < targetComponents.size();i++) {
		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		energy(component);
		rotate(component);
		translate(component);
		component->getTransform() = component->getTranslation()*component->getRotation()*component->getScale();
		std::cout << "Time: "<<TimeSystem::getTimeSinceStart()<<"\n";
		std::cout << "Pos: " << component->getTranslation()[3][0] << ", " << component->getTranslation()[3][1] << ", " << component->getTranslation()[3][2] << "\n";
		std::cout << "Vel: " << component->velocity.x << ", " << component->velocity.y << ", " << component->velocity.z << "\n\n";


		reset(component);
	}
}
#pragma endregion

#pragma region calculations
void PhysicsEngine::addForce(PhysicsComponent* _component, glm::vec3 force,glm::vec3 position) {
	_component->netForce += force;
	_component->netTorque += glm::cross(position, force);
}
void PhysicsEngine::addGravity(PhysicsComponent* _component) {
	_component->netForce += gravity*_component->gravityMultiplyer*_component->mass;
}
void PhysicsEngine::addAttractiveForces(PhysicsComponent* _component) {
	for (int i = 0; i < attractiveBodies.size();i++) {
	}
}

void PhysicsEngine::setAcceleration(PhysicsComponent* _component) {
	_component->dP = _component->netForce;
	_component->dL = _component->netTorque;

	
}
void PhysicsEngine::setMomentum(PhysicsComponent* _component) {
	_component->P += _component->dP;
	_component->L += _component->dL;
}

void PhysicsEngine::setVelocity(PhysicsComponent* _component) {

	_component->velocity = _component->P*TimeSystem::getPhysicsDeltaTime() / _component->mass;
	glm::mat4 inertia2 = _component->getRotation()*_component->momentOfInertia*glm::transpose(_component->getRotation());
	glm::mat4 inertia = _component->getRotation()*_component->momentOfInertiaInverse*glm::transpose(_component->getRotation());
	_component->angularVelocity = inertia*glm::vec4(_component->L, 0)*TimeSystem::getPhysicsDeltaTime();


	if (glm::length(_component->velocity) > MAX_SPEED ) {
		_component->velocity = glm::normalize(_component->velocity)*MAX_SPEED;
	}	


}

#pragma endregion

#pragma region application
void PhysicsEngine::translate(PhysicsComponent* _component) {
	glm::vec3 translation = _component->velocity*TimeSystem::getPhysicsDeltaTime();
	_component->getTranslation() = glm::translate(_component->getTranslation(), translation);
}
void PhysicsEngine::rotate(PhysicsComponent* _component) {
	float mag = glm::length(_component->angularVelocity);
	if (mag > 0) {
		_component->getRotation() = glm::rotate(_component->getRotation(),
			mag*TimeSystem::getPhysicsDeltaTime(),
			glm::normalize(_component->angularVelocity));
	}
	_component->getInverseRotation() = glm::inverse(_component->getRotation());
}
void PhysicsEngine::energy(PhysicsComponent* _component) {
	float speed = glm::length(_component->velocity);
	_component->kineticEnergy = (_component->mass*speed*speed/2.0f);
}

void PhysicsEngine::reset(PhysicsComponent* _component) {
		_component->dP = glm::vec3();
		_component->dL = glm::vec3();
		_component->netForce = glm::vec3();
		_component->netTorque = glm::vec3();
}

#pragma endregion
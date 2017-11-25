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
			 resolveCollisions();
		 }
	 }

 }

void PhysicsEngine::updatePhysics() {
	#pragma omp parallel for
	for (int i = 0; i < targetComponents.size();i++) {

		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		if (!component->getIsStatic()) {
			addGravity(component);
			setAcceleration(component);
			setMomentum(component);
			setVelocity(component);
		}
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
//		std::cout << "Time: "<<TimeSystem::getTimeSinceStart()<<"\n";
//		std::cout << "Pos: " << component->getTranslation()[3][0] << ", " << component->getTranslation()[3][1] << ", " << component->getTranslation()[3][2] << "\n";
//		std::cout << "Vel: " << component->velocity.x << ", " << component->velocity.y << ", " << component->velocity.z << "\n\n";


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

#pragma region collision_resolution
void PhysicsEngine::resolveCollisions() {

	for (int i = 0; i < CollisionEngine::getInstance()->collisionResults.size();i++) {
		float epsilon = 1E-6;
		CollisionResult & collision = *CollisionEngine::getInstance()->collisionResults[i];
		float vRel;
		Entity & entA = *collision.c1->entity;
		Entity & entB = *collision.c2->entity;
		glm::vec3 normalizedPenVector = glm::normalize(-collision.penetrationVector);
		glm::vec3 penVector = -collision.penetrationVector;

		PhysicsComponent & physA = (PhysicsComponent&)(*entA.getComponent(PHYSICS_COMPONENT));
		PhysicsComponent & physB = (PhysicsComponent&)(*entB.getComponent(PHYSICS_COMPONENT));
		vRel = glm::dot(normalizedPenVector,(physA.velocity - physB.velocity));

		if (vRel >epsilon)
			std::cout << "AWAY " << std::endl;
		if (abs(vRel) <=epsilon)
			std::cout << "RESTING " << std::endl;
		if (vRel < -epsilon)
		{

			float impulse;
			float coeffOfRestitution = 0.5f*(physA.coeffOfRestitution + physB.coeffOfRestitution);
			coeffOfRestitution = -(1 + coeffOfRestitution)*vRel;
		
			float massInverseSum = physA.massInverse + physB.massInverse;
		
			glm::vec3 rA = penVector;// collision.pointsC1[0];
			glm::vec3 rB = -penVector;//collision.pointsC2[0];

			glm::vec4 IrAXN = physA.momentOfInertiaInverse*glm::vec4(glm::cross(rA, normalizedPenVector),0);
			glm::vec4 IrBXN = physB.momentOfInertiaInverse*glm::vec4(glm::cross(rB, normalizedPenVector),0);
			
			glm::vec3 rAX = glm::cross(glm::vec3(IrAXN.x, IrAXN.y, IrAXN.z), rA);
			glm::vec3 rBX = glm::cross(glm::vec3(IrBXN.x, IrBXN.y, IrBXN.z), rB);

			float dotA = glm::dot(normalizedPenVector, rAX);
			float dotB = glm::dot(normalizedPenVector, rBX);

			impulse = coeffOfRestitution / (dotA + dotB + massInverseSum);
			glm::vec3 impulseVector = glm::normalize(impulse*penVector);

			physA.P = physB.mass*impulseVector*physA.coeffOfRestitution*glm::distance(physA.velocity,glm::vec3());
			physB.P = physA.mass*impulseVector*physB.coeffOfRestitution*glm::distance(physB.velocity,glm::vec3());
		std::cout << "CUBE VECTOR " << rA.x << ", " << rA.y << ", " << rA.z << std::endl;
		std::cout << "CYLINDER VECTOR " << rB.x << ", " << rB.y << ", " << rB.z << std::endl;

		}

	}
}

#pragma endregion
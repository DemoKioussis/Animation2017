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
#define MIN_ENERGY 0.05f
#define epsilon 1E-6f
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
	//	 if (TimeSystem::physicsCheck() >= updateTime) {
			 TimeSystem::physicsStep();			 
			 updatePhysics();
			 applyPhysics();
			 CollisionEngine::getInstance()->step(); // Collision detection should only happen at the physical step
			 resolveCollisions();
		// }
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
			energy(component);
		}
	}
}

void PhysicsEngine::applyPhysics() {
	#pragma omp parallel for
	for (int i = 0; i < targetComponents.size();i++) {
		PhysicsComponent* component = (PhysicsComponent*)targetComponents[i];
		if (!component->getIsStatic() || component->kineticEnergy>MIN_ENERGY) {
			rotate(component);
			translate(component);
			component->getTransform() = component->getTranslation()*component->getRotation()*component->getScale();

			reset(component);
		}
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
		CollisionResult & collision = *CollisionEngine::getInstance()->collisionResults[i];

		Entity & entA = *collision.c1->entity;
		Entity & entB = *collision.c2->entity;

		glm::vec3 normalizedPenVector = glm::normalize(-collision.penetrationVector);
		glm::vec3 penVector = -collision.penetrationVector;
	//	printf("\npenVector %d: %f , %f, %f\n",i, penVector.x, penVector.y, penVector.z);


		PhysicsComponent & physA = (PhysicsComponent&)(*entA.getComponent(PHYSICS_COMPONENT));
		PhysicsComponent & physB = (PhysicsComponent&)(*entB.getComponent(PHYSICS_COMPONENT));

		float massInverseSum = 0;
		float coeffOfRestitution = 0.5f*(physA.coeffOfRestitution + physB.coeffOfRestitution);

		if (!physA.isStatic) massInverseSum += physA.massInverse*8;
		if (!physB.isStatic) massInverseSum += physB.massInverse*8;

		if (physA.isStatic && physB.isStatic)
			continue;

		vector<glm::vec3> impulses(collision.points1LC.size() + collision.points2LC.size());
		glm::vec3 netImpulse(0);

//#pragma omp parallel for
		for (int j = 0; j < collision.points1LC.size();j++) {
			glm::vec3 worldPos = physA.getTransform()*glm::vec4(collision.points1OC[j], 1);


			glm::vec3 rA = collision.points1LC[j];
			glm::vec3 rB = physB.getScale()*glm::inverse(physB.getTransform())*physA.getTransform()*glm::vec4(collision.points1OC[j],1);
			glm::vec3 velA = physA.velocity + glm::cross(physA.angularVelocity, rA);
			glm::vec3 velB = physB.velocity + glm::cross(physB.angularVelocity, rB);


			float vRel =  glm::dot(normalizedPenVector,(velA - velB));
	//		printf("\nA %d/%d - vrel%f:\n", j+1, collision.points1LC.size(),vRel);
	//		printf("vA: %f , %f, %f\n", velA.x, velA.y, velA.z);
	//		printf("vB: %f , %f, %f\n", velB.x, velB.y, velB.z);


	//		printf("posA: %f , %f, %f\n", rA.x, rA.y, rA.z);
	//		printf("posB: %f , %f, %f\n", rB.x, rB.y, rB.z);

	//		printf("World pos: %f , %f, %f\n\n", worldPos.x, worldPos.y, worldPos.z);

			if (vRel > epsilon) {
			//	std::cout << "AWAY " << std::endl;
			}
			if (abs(vRel) <= epsilon || glm::length(penVector) < 0.0025f) {

			//	std::cout << "RESTING " << std::endl;
			}
			if (vRel < -epsilon) {
			//	std::cout << "COLLISION " << std::endl;
				impulses[j] = getImpulse(rA, rB, vRel, physA, physB,massInverseSum,penVector,normalizedPenVector, coeffOfRestitution)
					/ (float)collision.points1LC.size();
			}

		}
//#pragma omp parallel for
		for (int k = 0; k < collision.points2LC.size();k++) {
			glm::vec3 worldPos = physB.getTransform()*glm::vec4(collision.points2OC[k], 1);
			glm::vec3 rB = collision.points2LC[k];
			glm::vec3 rA = physA.getScale()*glm::inverse(physA.getTransform())*physB.getTransform()*glm::vec4(collision.points2OC[k], 1);
			glm::vec3 velA = physA.velocity + glm::cross(physA.angularVelocity, rA);
			glm::vec3 velB = physB.velocity + glm::cross(physB.angularVelocity, rB);
			float vRel = glm::dot(normalizedPenVector, (velA - velB));

	//		printf("\nB %d/%d - vrel%f:\n", k+1, collision.points2LC.size(),vRel);
	//		printf("vA: %f , %f, %f\n", velA.x, velA.y, velA.z);
	//		printf("vB: %f , %f, %f\n", velB.x, velB.y, velB.z);

	//		printf("posA: %f , %f, %f\n", rA.x, rA.y, rA.z);
	//		printf("posB: %f , %f, %f\n", rB.x, rB.y, rB.z);
			
	//		printf("World pos: %f , %f, %f\n\n", worldPos.x, worldPos.y, worldPos.z);


			if (vRel > epsilon) {
			//	std::cout << "AWAY " << std::endl;
			}
			if (abs(vRel) <= epsilon || glm::length(penVector) < 0.0025f) {

			//	std::cout << "RESTING " << std::endl;
			}
			if (vRel < -epsilon) {
			//	std::cout << "COLLISION " << std::endl;

				impulses[k+ collision.points1LC.size()] = getImpulse(rA, rB, vRel, physA, physB, massInverseSum, penVector, normalizedPenVector, coeffOfRestitution)
					/(float) collision.points2LC.size();
			}
		}
		for (int i = 0; i < impulses.size();i++) {
			netImpulse += impulses[i];
		}
		netImpulse /= TimeSystem::getPhysicsDeltaTime();
		physA.P += netImpulse;
		physB.P -= netImpulse;
	}


}

glm::vec3 PhysicsEngine::getImpulse(glm::vec3 &rA, glm::vec3 &rB, float vRel, PhysicsComponent & physA, PhysicsComponent & physB,
	float massInverseSum, glm::vec3 &penVector, glm::vec3 & normalizedPenVector, float coeffOfRestitution) {
	
	float impulse;

	glm::vec4 IrAXN = physA.momentOfInertiaInverse*glm::vec4(glm::cross(rA, normalizedPenVector), 0);
	glm::vec4 IrBXN = physB.momentOfInertiaInverse*glm::vec4(glm::cross(rB, normalizedPenVector), 0);

	glm::vec3 rAX = glm::cross(glm::vec3(IrAXN.x, IrAXN.y, IrAXN.z), rA);
	glm::vec3 rBX = glm::cross(glm::vec3(IrBXN.x, IrBXN.y, IrBXN.z), rB);

	float dotA, dotB;

	if (physA.isStatic) dotA = 0;
	else dotA = glm::dot(normalizedPenVector, rAX);

	if (physB.isStatic) dotB = 0;
	else dotB = glm::dot(normalizedPenVector, rBX);


	impulse = coeffOfRestitution / (dotA + dotB + massInverseSum);
	glm::vec3 impulseVector = impulse*glm::normalize(penVector) / TimeSystem::getPhysicsDeltaTime();
//	physA.P += impulseVector*250.0f;
//	physB.P -= impulseVector*250.0f;
	physA.L += glm::cross(rA, impulseVector);
	physB.L -= glm::cross(rB, impulseVector);

	return impulseVector;
}

void PhysicsEngine::resolveCollisions2() {

	for (int i = 0; i < CollisionEngine::getInstance()->collisionResults.size();i++) {

		CollisionResult & collision = *CollisionEngine::getInstance()->collisionResults[i];
		float vRel;
		Entity & entA = *collision.c1->entity;
		Entity & entB = *collision.c2->entity;
		glm::vec3 normalizedPenVector = glm::normalize(-collision.penetrationVector);
		glm::vec3 penVector = -collision.penetrationVector;

		PhysicsComponent & physA = (PhysicsComponent&)(*entA.getComponent(PHYSICS_COMPONENT));
		PhysicsComponent & physB = (PhysicsComponent&)(*entB.getComponent(PHYSICS_COMPONENT));
		if (physA.isStatic && physB.isStatic)
			continue;
		vRel = glm::dot(normalizedPenVector,(physA.velocity - physB.velocity)/TimeSystem::getPhysicsDeltaTime());
		std::cout << "vrel: " << vRel << std::endl;
		std::cout << "penMAg: " << glm::length(penVector)<<"\n\n" << std::endl;

		if (vRel > epsilon) {
				std::cout << "AWAY " << std::endl;
		}
		if (abs(vRel) <= epsilon || glm::length(penVector) < 0.0025f) {
			
			std::cout << "resting " << std::endl;


		
		}

		if (vRel < -epsilon)
		{
			std::cout << "Collision " << std::endl;

			float impulse;
			float coeffOfRestitution = 0.5f*(physA.coeffOfRestitution + physB.coeffOfRestitution);
			coeffOfRestitution = -(1 + coeffOfRestitution)*vRel;
		
			float massInverseSum = 0; 
			
			if (!physA.isStatic) massInverseSum += physA.massInverse; 
			if (!physB.isStatic) massInverseSum += physB.massInverse;
		
			glm::vec3 rA = penVector;// collision.pointsC1[0];
			glm::vec3 rB = -penVector;//collision.pointsC2[0];

			glm::vec4 IrAXN = physA.momentOfInertiaInverse*glm::vec4(glm::cross(rA, normalizedPenVector),0);
			glm::vec4 IrBXN = physB.momentOfInertiaInverse*glm::vec4(glm::cross(rB, normalizedPenVector),0);
			
			glm::vec3 rAX = glm::cross(glm::vec3(IrAXN.x, IrAXN.y, IrAXN.z), rA);
			glm::vec3 rBX = glm::cross(glm::vec3(IrBXN.x, IrBXN.y, IrBXN.z), rB);

			float dotA, dotB;

			if (physA.isStatic) dotA = 0;
			else dotA = glm::dot(normalizedPenVector, rAX);

			if (physB.isStatic) dotB = 0;
			else dotB = glm::dot(normalizedPenVector, rBX);


			impulse = coeffOfRestitution / (dotA + dotB + massInverseSum);
			glm::vec3 impulseVector = impulse*glm::normalize(penVector);

			physA.P += impulseVector;
			physB.P -= impulseVector;
	//		physA.getTranslation() = glm::translate(physA.getTranslation(), penVector/2.0f);
	//		physB.getTranslation() = glm::translate(physB.getTranslation(), -penVector/2.0f);
			auto makeVec3 = [&](glm::vec4 & v)->glm::vec3 
			{
				return glm::vec3(v.x, v.y, v.z);
			};
			for (int i = 0; i < collision.pointsOf2Inside1LC.size(); i++) {
				physA.L += glm::cross(collision.pointsOf2Inside1LC[i], impulseVector) /(float) collision.pointsOf2Inside1LC.size()*0.05f;
			}
			for (int i = 0; i < collision.pointsOf1Inside2LC.size(); i++) {
				physB.L -= glm::cross(collision.pointsOf1Inside2LC[i], impulseVector) / (float) collision.pointsOf1Inside2LC.size()*0.05f;
			}
		}
	}
}

#pragma endregion
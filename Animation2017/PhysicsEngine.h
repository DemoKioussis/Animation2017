#pragma once
#include "Engine.h"
#include <glad\glad.h>
#include <glm\glm.hpp>
#include "glm\gtx\matrix_operation.hpp"

#define MAX_SPEED 10000.0f
#define REST_ENERGY 0.01f

class Mesh;
class PhysicsComponent;
class PhysicsEngine : public Engine {

public:
	std::vector<Mesh*> meshes;
	static void Initialize();
	static void Clear();
	static PhysicsEngine * getInstance();
	void updatePhysics();
	void applyPhysics();
	glm::vec3 getForce(PhysicsComponent* _component);
	void addAttractor(PhysicsComponent* _component);
	void setGravity(glm::vec3 _dir, float _mag);
	glm::vec3 getGravity();
	void step();
	void addForce(PhysicsComponent* _component, glm::vec3 _force, glm::vec3 _position);
	void setForce(PhysicsComponent* _component, glm::vec3 _force, glm::vec3 _position);
	float updateTime;
	void setVelocity(PhysicsComponent* _component);
	void setTorque(PhysicsComponent* _component, glm::vec3 torque);
	void addTorque(PhysicsComponent* _component, glm::vec3 torque);
private:
	static PhysicsEngine* instance;

	void addGravity(PhysicsComponent* _component);
	void addAttractiveForces(PhysicsComponent* _component);
	void setAcceleration(PhysicsComponent* _component);
	void setMomentum(PhysicsComponent* _component);
	void resolveCollisions();
	glm::vec3 getImpulse(glm::vec3 & _rA, glm::vec3 & _rB, float _vRel, PhysicsComponent & _physA, PhysicsComponent & _physB,
		float _massInverseSum,glm::vec3 & _penVector, glm::vec3 & _normalizePenVector, float _coeffOfRestitution, int _numPoints);
	void resolveCollisions2();
	
	void energy(PhysicsComponent* _component);
	void translate(PhysicsComponent* _component);
	void rotate(PhysicsComponent* _component);
	void reset(PhysicsComponent* _component);

	bool gravityEnabled;
	glm::vec3 gravity;
	std::vector<PhysicsComponent* > attractiveBodies;
};

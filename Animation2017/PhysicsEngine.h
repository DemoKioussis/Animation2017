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

	void addAttractor(PhysicsComponent* _component);
	void setGravity(glm::vec3 _dir, float _mag);
	glm::vec3 getGravity();
	void step();
	void addForce(PhysicsComponent* _component, glm::vec3 _force, glm::vec3 _position);
	float updateTime;

private:
	static PhysicsEngine* instance;

	void addGravity(PhysicsComponent* _component);
	void addAttractiveForces(PhysicsComponent* _component);
	void setAcceleration(PhysicsComponent* _component);
	void setMomentum(PhysicsComponent* _component);
	void setVelocity(PhysicsComponent* _component);
	void resolveCollisions();
	void energy(PhysicsComponent* _component);
	void translate(PhysicsComponent* _component);
	void rotate(PhysicsComponent* _component);
	void reset(PhysicsComponent* _component);

	bool gravityEnabled;
	glm::vec3 gravity;
	std::vector<PhysicsComponent* > attractiveBodies;
};

#pragma once
#include "Engine.h"
#include <glad\glad.h>
#include <glm\glm.hpp>

#define MAX_SPEED 10000.0f
#define REST_ENERGY 0.01f

class PhysicsComponent;
class PhysicsEngine : public Engine {

public:
	static void Initialize();
	static PhysicsEngine * getInstance();
	void updatePhysics();
	void applyPhysics();

	void addAttractor(PhysicsComponent* _component);
	void setGravity(glm::vec3 _dir, float _mag);
	void step();
	void addForce(PhysicsComponent* _component, glm::vec3 _force, glm::vec3 _position);

private:
	static PhysicsEngine* instance;

	void addGravity(PhysicsComponent* _component);
	void addAttractiveForces(PhysicsComponent* _component);
	void setAcceleration(PhysicsComponent* _component);
	void setVelocity(PhysicsComponent* _component);

	void energy(PhysicsComponent* _component);
	void momentum(PhysicsComponent* _component);
	void translate(PhysicsComponent* _component);
	void rotate(PhysicsComponent* _component);

	bool gravityEnabled;
	glm::vec3 gravity;
	std::vector<PhysicsComponent* > attractiveBodies;
};

#include "TimeSystem.h"


float TimeSystem::elapsedTime = 0;


float TimeSystem::lastFrameStep = 0;
float TimeSystem::lastPhysicsStep = 0;


float TimeSystem::frameDeltaTime = 0;
float TimeSystem::physicsDeltaTime = 0;


float TimeSystem::timeScale = 0;


int TimeSystem::fpsCounter = 0;
float TimeSystem::fpsTracker = 0;
void TimeSystem::begin() {

	elapsedTime = glfwGetTime();
	lastFrameStep = elapsedTime;
	lastPhysicsStep = elapsedTime;
	frameDeltaTime = 0;
	physicsDeltaTime = 0;
};

void TimeSystem::update() {

	elapsedTime = glfwGetTime();
};

float TimeSystem::physicsCheck() {
	return elapsedTime - lastPhysicsStep;;
};

void TimeSystem::physicsStep() {
	physicsDeltaTime = elapsedTime - lastPhysicsStep;
	lastPhysicsStep = elapsedTime;
};


float TimeSystem::frameCheck() {
	return elapsedTime - lastFrameStep;
};
void TimeSystem::frameStep() {
	frameDeltaTime = elapsedTime - lastFrameStep;
	lastFrameStep = elapsedTime;
	fpsCounter++;
	if (elapsedTime - fpsTracker >= 1) {
		fpsTracker = elapsedTime;
		std::cout << "frames this second: " << fpsCounter << std::endl;
		fpsCounter = 0;
	}
};

void TimeSystem::setTimeScale(float t) {
	timeScale = t;
};
float TimeSystem::getFrameDeltaTime() {
	return frameDeltaTime;
};
float TimeSystem::getPhysicsDeltaTime() {
	return physicsDeltaTime;
};

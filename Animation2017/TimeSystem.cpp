#include "TimeSystem.h"


float TimeSystem::elapsedTime = 0;


float TimeSystem::lastFrameStep = 0;
float TimeSystem::lastPhysicsStep = 0;


float TimeSystem::frameDeltaTime = 0;
float TimeSystem::physicsDeltaTime = 0;


float TimeSystem::timeScale = 1;
float TimeSystem::physicsTimeFactor = 0.0165;


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

void TimeSystem::resetTime() {
	glfwSetTime(0);
	elapsedTime = 0;
	lastFrameStep = 0;
	lastPhysicsStep = 0;
	frameDeltaTime = 0;
	physicsDeltaTime = 0;
	std::cout << "reset time" << std::endl;
}
float TimeSystem::physicsCheck() {
	return elapsedTime - lastPhysicsStep;
};

void TimeSystem::physicsStep() {
	physicsDeltaTime = (elapsedTime - lastPhysicsStep)*timeScale;
	lastPhysicsStep = elapsedTime;
	if (physicsDeltaTime > 0.0167) {
	//	std::cout << "PROBLEM WITH DELTA TIME: " << physicsDeltaTime << std::endl;

		physicsDeltaTime = 0.0165*timeScale;
	}

	//	physicsDeltaTime = physicsTimeFactor*timeScale;
	//}

};
float TimeSystem::getPhysicsTimeFactor() {
	return physicsTimeFactor;
}

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


float TimeSystem::getFrameDeltaTime() {
	return frameDeltaTime;
};
float TimeSystem::getPhysicsDeltaTime() {
	return physicsDeltaTime;//timeScale / 120.0f;
};


const float TimeSystem::getTimeSinceStart() {
	return elapsedTime;
}

void TimeSystem::setTimeScale(float t) {
	timeScale = t;
	if (timeScale < 0)
		timeScale = 0;

	std::cout << "Time scale = " << timeScale << std::endl;
	
};

const float TimeSystem::getTimeScale() {
	return timeScale;
};
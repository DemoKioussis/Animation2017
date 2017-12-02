#pragma once

#include <iostream>
#include <GLFW\glfw3.h>
class TimeSystem {
	static float elapsedTime;


	static float lastFrameStep;
	static float lastPhysicsStep;

	
	static float frameDeltaTime;
	static float physicsDeltaTime;
	static float timeScale;
	static float physicsTimeFactor;
	static int fpsCounter;
	static float fpsTracker;
public:
	static void begin();
	static void update();
	static void resetTime();
	static float physicsCheck();
	static void physicsStep();

	static float frameCheck();
	static void frameStep();

	static float getFrameDeltaTime();
	static float getPhysicsDeltaTime();

	static const float getTimeSinceStart();

	static void setTimeScale(float t);
	static const float getTimeScale();
	static float getPhysicsTimeFactor();
};
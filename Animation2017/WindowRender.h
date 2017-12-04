#pragma once

#include "WindowManager.h"
#include "GUIManager.h"
#include "Camera.h"
#include "PhysicsSimulation.h"

class WindowRender
{
public:
	static void Initialize();
	void Render();
	static WindowRender* getInstance();



private:
	static WindowRender* instance;
};


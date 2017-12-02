
#include "PhysicsSimulation.h"
#include "WindowRender.h"


int main()
{


	PhysicsSimulation::Initialize();

#pragma region mainLoop
	while (PhysicsSimulation::getInstance()->shouldWindowClose())
	{
		WindowRender::getInstance()->Render();
	}

#pragma endregion
	//destroy simulation here
	glfwTerminate();
	return 0;
}






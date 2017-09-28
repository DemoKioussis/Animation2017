#pragma once


#include "Renderable.h"
#include <vector>
#include "Shader.h"

class RenderEngine {

public:
	static Shader* currentShader;
	static void addRenderTarget(Renderable &renderingTarget);
	static void drawRenderTargets();
	static void enable();
	static void disable();
	static bool isEnabled();
private:
	static std::vector<Renderable*> renderingTargets;
	static bool enabled;



};
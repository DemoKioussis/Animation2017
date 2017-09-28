#include "RenderEngine.h"

bool RenderEngine::enabled = true;
std::vector<Renderable*> RenderEngine::renderingTargets(0);

void RenderEngine::enable() {
	enabled = true;
}

void RenderEngine::disable() {
	enabled = false;
}

bool RenderEngine::isEnabled() {
	return enabled;
}

void RenderEngine::drawRenderTargets() {
	for (int i = 0; i < renderingTargets.size();i++) {
		renderingTargets[i]->draw();
	}
}

void RenderEngine::addRenderTarget(Renderable &renderable) {
	renderingTargets.push_back(&renderable);
}
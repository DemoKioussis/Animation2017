#include "RenderEngine.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "Shader.h"

//bool RenderEngine::enabled = true;
//std::vector<Renderable*> RenderEngine::renderReferences(0);

RenderEngine* RenderEngine::instance = 0;

void RenderEngine::Initialize() {
	if (instance)
		return;
	RenderEngine * engine = new RenderEngine();
	instance = engine;
}
void RenderEngine::setShader(Shader* _shader) {
	shader = _shader;
	projLoc = glGetUniformLocation(shader->ID, "projection");
	viewLoc = glGetUniformLocation(shader->ID, "view");
	modelLoc = glGetUniformLocation(shader->ID, "model");
}
void RenderEngine::drawRenderComponents() {
	for (int i = 0; i < targetComponents.size();i++) {
		Component* component = targetComponents[i];
		int meshID = ((RenderComponent*)component)->getMeshID();
		shader->setMat4(modelLoc,component->getTransform());
		renderReferences[meshID]->draw();
	}
}

void RenderEngine::addRenderReference(Renderable * _renderReference) {
	renderReferences.push_back(_renderReference);
}

std::vector<Renderable*>& RenderEngine::getAllRenderReferences()
{
	return renderReferences;
}

void RenderEngine::draw(int _renderIndex) {
	renderReferences[_renderIndex]->draw();
	
}

RenderEngine* RenderEngine::getInstance() {
	return instance;
}
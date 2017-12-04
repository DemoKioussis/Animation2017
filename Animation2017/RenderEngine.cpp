#include "RenderEngine.h"
#include "Mesh.h"
#include "RenderComponent.h"
#include <glm\gtx\matrix_operation.hpp>
#include <glm\gtc\matrix_transform.hpp>
//bool RenderEngine::enabled = true;

RenderEngine* RenderEngine::instance = 0;

void RenderEngine::Initialize() {
	if (instance)
		return;
	RenderEngine * engine = new RenderEngine();
	instance = engine;
	engine->colorsSet = false;
	engine->enable();
}
void RenderEngine::Clear() {
	for (int i = 0; i < instance->sortedRenderCompoents.size(); i++) {
		if (instance->sortedRenderCompoents[i] != nullptr) {
			instance->sortedRenderCompoents[i]->clear();
		}
	}
	instance->targetComponents.clear();
}

void RenderEngine::setShader(Shader* _shader) {
	shader = _shader;
	projLoc = glGetUniformLocation(shader->ID, "projection");
	viewLoc = glGetUniformLocation(shader->ID, "view");
	modelLoc = glGetUniformLocation(shader->ID, "model");
}
Shader* RenderEngine::getShader() {
	return instance->shader;
}
void RenderEngine::addComponent(Component* _renderComponent) {
	Engine::addComponent(_renderComponent);

	int mesID = ((RenderComponent*)_renderComponent)->getMeshID();
	sortedRenderCompoents[mesID]->push_back((RenderComponent*)_renderComponent);
	colorsSet = false;
}
void RenderEngine::updateColors() {
	for (int i = 0; i < sortedRenderCompoents.size();i++) {

		vector<glm::vec3> colorData;
		for (int j = 0; j < sortedRenderCompoents[i]->size();j++) {

			glm::vec3 & color = sortedRenderCompoents[i]->at(j)->getColor();
			colorData.push_back(color);
		}
		renderReferences[i]->setColorBuffer(colorData);
	}
	colorsSet = true;

}
void RenderEngine::drawRenderComponents() {

	if (!isEnabled())
		return;
#ifndef USE_INSTANCING

	for (int i = 0; i < targetComponents.size();i++) {
		Component* component = targetComponents[i];
		int meshID = ((RenderComponent*)component)->getMeshID();
		shader->setMat4(modelLoc,component->getTransform());
		renderReferences[meshID]->draw();
	}


#else

	if (!colorsSet) updateColors();
	for (int i = 0; i < sortedRenderCompoents.size();i++) {
		
		vector<glm::mat4> data;
		for (int j = 0; j < sortedRenderCompoents[i]->size();j++) {
			if (sortedRenderCompoents[i]->at(j)->isEnabled()) {
				glm::mat4 & trans = sortedRenderCompoents[i]->at(j)->getTransform();
				data.push_back(trans);
			}
			else {
				glm::vec3 trans(100000.0f, 100000.0f, 100000.0f);
				glm::mat4 transMat = glm::translate(glm::mat4(1.0f), trans);
				data.push_back(transMat);
			}
		}

		renderReferences[i]->setBufferData(data);
		renderReferences[i]->numPrims = sortedRenderCompoents[i]->size();
	}
	for (int i = 0; i < renderReferences.size();i++) {
		renderReferences[i]->draw();
	}

#endif
}

void RenderEngine::addRenderReference(Mesh * _renderReference) {
	renderReferences.push_back(_renderReference);
	std::vector<RenderComponent*> *vec = new std::vector<RenderComponent*>();
	sortedRenderCompoents.push_back(vec);
}

std::vector<Mesh*>& RenderEngine::getAllRenderReferences()
{
	return renderReferences;
}

void RenderEngine::draw(int _renderIndex) {
	renderReferences[_renderIndex]->draw();
	
}

RenderEngine* RenderEngine::getInstance() {
	return instance;
}
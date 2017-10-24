#include "RenderEngine.h"
#include "Mesh.h"
#include "RenderComponent.h"
#include "Shader.h"

//bool RenderEngine::enabled = true;

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
void RenderEngine::addComponent(Component* _renderComponent) {
	Engine::addComponent(_renderComponent);

	int mesID = ((RenderComponent*)_renderComponent)->getMeshID();
	sortedRenderCompoents[mesID]->push_back((RenderComponent*)_renderComponent);
}
void RenderEngine::drawRenderComponents() {
#ifndef USE_INSTANCING

	for (int i = 0; i < targetComponents.size();i++) {
		Component* component = targetComponents[i];
		int meshID = ((RenderComponent*)component)->getMeshID();
		shader->setMat4(modelLoc,component->getTransform());
		renderReferences[meshID]->draw();
	}


#else	
	for (int i = 0; i < sortedRenderCompoents.size();i++) {
		
		vector<glm::mat4> data;
		for (int j = 0; j < sortedRenderCompoents[i]->size();j++) {
		
			glm::mat4 & trans = sortedRenderCompoents[i]->at(j)->getTransform();
				data.push_back(trans);
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
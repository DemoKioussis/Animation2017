#include "RenderEngine.h"
#include "Mesh.h"
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
void RenderEngine::addComponent(Component* _renderComponent) {
	Engine::addComponent(_renderComponent);

	int mesID = ((RenderComponent*)_renderComponent)->getMeshID();
	sortedRenderCompoents[mesID]->push_back((RenderComponent*)_renderComponent);
}
void RenderEngine::drawRenderComponents() {
//	for (int i = 0; i < targetComponents.size();i++) {
//		Component* component = targetComponents[i];
//		int meshID = ((RenderComponent*)component)->getMeshID();
//		shader->setMat4(modelLoc,component->getTransform());
//		renderReferences[meshID]->draw();
//	}
	


	for (int i = 0; i < sortedRenderCompoents.size();i++) {
		
		vector<glm::mat4> data;
		for (int j = 0; j < sortedRenderCompoents[i]->size();j++) {
		
			glm::mat4 & trans = sortedRenderCompoents[i]->at(j)->getTransform();
				data.push_back(trans);
		}

		glBindVertexArray(renderReferences[i]->VAO);
		glBindBuffer(GL_ARRAY_BUFFER,renderReferences[i]->transformBuffer);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), &data[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		renderReferences[i]->numPrims = sortedRenderCompoents[i]->size();
	}
	for (int i = 0; i < renderReferences.size();i++) {
		renderReferences[i]->draw();
	}
	
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
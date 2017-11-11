#pragma once
#include "Engine.h"
#include "Shader.h"
#include <glad\glad.h>
class Mesh;
class Shader;
class RenderComponent;
class RenderEngine : public Engine{

public:
	static void Initialize();
	static void Refresh(Shader* shaderProg);
	void setShader(Shader* _shader);
	Shader* getShader();
	void addRenderReference(Mesh *_renderReference);
	std::vector<Mesh*>& getAllRenderReferences();
	std::vector<Mesh*> renderReferences;
	void addComponent(Component *_renderComponent);
	void drawRenderComponents();
	static RenderEngine * getInstance();
private:
	static RenderEngine* instance;
	Shader* shader;
	GLuint projLoc;
	GLuint viewLoc;
	GLuint modelLoc;
	void draw(int _renderIndex);

	std::vector<std::vector<RenderComponent*>*> sortedRenderCompoents;
};
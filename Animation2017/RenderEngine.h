#pragma once
#include "Engine.h"
#include <glad\glad.h>e
class Renderable;
class Shader;
class RenderEngine : public Engine{

public:
	static void Initialize();
	void setShader(Shader* _shader);
	void addRenderReference(Renderable *_renderReference);
	std::vector<Renderable*> renderReferences;
//	void addComponent(RenderComponent *_renderComponent);
	void drawRenderComponents();
	static RenderEngine * getInstance();
private:
	static RenderEngine* instance;
	Shader* shader;
	GLuint projLoc;
	GLuint viewLoc;
	GLuint modelLoc;
	void draw(int _renderIndex);
};
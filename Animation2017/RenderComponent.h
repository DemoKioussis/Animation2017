#pragma once
#include "Component.h"

class Entity;
class RenderComponent : public Component {
private:
	int renderReference;
	glm::vec3 color;
public:
	RenderComponent(glm::vec3 c = glm::vec3(0.5f,0.5f,0.5f)) {
		componentType = RENDER_COMPONENT;
		color = c;
		enable();
	};
	~RenderComponent() {};

	const int getMeshID() {
		return renderReference;
	};
	void setMeshID(int _i) {
		renderReference = _i;
	};
	glm::vec3 getColor() {
		return color;
	}
	void setColor(glm::vec3 c) {
		color = c;
	}
};
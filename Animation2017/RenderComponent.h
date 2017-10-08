#pragma once
#include "Component.h"

class Entity;
class RenderComponent : public Component {
private:
	int renderReference;
public:
	RenderComponent() {
		componentType = RENDER_COMPONENT;
	};
	~RenderComponent() {};

	const int getMeshID() {
		return renderReference;
	};
	void setMeshID(int _i) {
		renderReference = _i;
	};
};
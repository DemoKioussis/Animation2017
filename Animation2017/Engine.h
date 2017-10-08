#pragma once
#include <vector>

class Component;
class Engine {
public:
private:
	bool enabled;
protected:
	std::vector<Component*> targetComponents;
	Engine() {}
	~Engine() {}
//	virtual void Initialize() {};
public:
	void enable() {
		enabled = true;
	}
	void disable() {
		enabled = false;
	}
	const bool isEnabled() {
		return enabled;
	}
	virtual void addComponent(Component* c) {
		targetComponents.push_back(c);
	}
};
#pragma once
#include <vector>
#include <glm\glm.hpp>
class Component;
class Engine;
class Entity {
	friend class Component;
	friend class Engine;
public:
	std::vector<Component*> components;
	glm::mat4 transform;

	Entity();
	~Entity();
	void addComponent(Component *_component);
	Component* getComponent(int _componentType);
	void removeComponent(int _componentType);
	const int getID();
private:
	static int idCounter;
	int id;
};
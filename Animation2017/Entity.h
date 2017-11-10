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
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 inverseRotation;

	Entity(bool _isStatic);
	~Entity();
	void addComponent(Component *_component);
	Component* getComponent(int _componentType);
	void removeComponent(int _componentType);
	const int getID();
	bool isStatic();
private:
	static int idCounter;
	int id;
	bool isStaticEntity;
};
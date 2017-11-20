#include "Entity.h"
#include "Component.h"
#include "iostream"

int Entity::idCounter = 0;
Entity::Entity(bool _isStatic) : isStaticEntity(_isStatic)
{
	id = idCounter++;
	transform = glm::mat4(1.0f);
	scale = glm::mat4(1.0f);
	rotation = glm::mat4(1.0f);
	translation = glm::mat4(1.0f);
}
Entity::~Entity() {
	//delete all components;
}
void Entity::addComponent(Component* _component) {
	if (!getComponent(_component->getType())) {
		components.push_back(_component);
		_component->setEntity(this);
	}
	else
		std::cout << "Entity " << id << "already has a component of this type" << std::endl;
}
Component* Entity::getComponent(int _componentType) {
	for (int i = 0; i < components.size();i++) {
		if (components[i]->getType() == _componentType)
			return components[i];
	}
	return 0;
};
void Entity::removeComponent(int _componentType) {

};

const int Entity::getID() {
	return id;
}

bool Entity::isStatic()
{
	return isStaticEntity;
}

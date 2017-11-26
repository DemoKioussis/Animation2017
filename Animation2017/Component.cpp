#include "Component.h"
#include "Entity.h"

Component::Component() {
	componentType = COMPONENT;
}
Component::~Component() {
	//stuff
}

const ComponentType Component::getType() {
	return componentType;
}

void Component::setEntity(Entity* e) {
	entity = e;
}

void Component::enable() {
	enabled = true;
}

void Component::disable() {
	enabled = false;
}
const bool Component::isEnabled() {
	return enabled;
}
glm::mat4 & Component::getTransform() {
	return entity->transform;
}
glm::mat4 & Component::getUpdatedTransform() {
	return entity->transform*entity->rotation*entity->scale;
}
glm::mat4 & Component::getRotation() {
	return entity->rotation;
}
glm::mat4 & Component::getInverseRotation() {
	return entity->inverseRotation;
}
glm::mat4 & Component::getTranslation() {
	return entity->translation;
}
glm::mat4 & Component::getScale() {
	return entity->scale;
}
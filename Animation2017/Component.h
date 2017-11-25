#pragma once
#include <glm\glm.hpp>
enum ComponentType
{
	COMPONENT,RENDER_COMPONENT,PHYSICS_COMPONENT, COLLISION_COMPONENT
};

class Component {
	friend class Entity;
private:
	bool enabled;
protected:
	ComponentType componentType;
public:
	
	Component();
	~Component();
	Entity* entity;
	const ComponentType getType();
	
	void setEntity(Entity* e);
	void enable();
	void disable();
	const bool isEnabled();
	glm::mat4 & getTransform();
	glm::mat4 & getRotation();
	glm::mat4 & getInverseRotation();
	glm::mat4 & getTranslation();
	glm::mat4 & getScale();
	glm::mat4 & Component::getUpdatedTransform();

};
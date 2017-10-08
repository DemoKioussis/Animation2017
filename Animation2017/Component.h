#pragma once
#include <glm\glm.hpp>
enum ComponentType
{
	COMPONENT,RENDER_COMPONENT
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
	const glm::mat4 & getTransform();


};
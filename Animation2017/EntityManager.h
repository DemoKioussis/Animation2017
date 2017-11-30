#pragma once
#include <vector>
#include "Entity.h"
class EntityManager
{

public:
	static EntityManager* getInstance();
	static void Initialize();
	~EntityManager();
	void add(Entity* entity);
	std::vector<Entity*>* getEntities();
private:
	EntityManager();
	static EntityManager* instance;
	std::vector<Entity*> *entities = new std::vector<Entity*>();
};


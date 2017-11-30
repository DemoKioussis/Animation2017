#include "EntityManager.h"
EntityManager* EntityManager::instance = nullptr;


EntityManager::EntityManager()
{
}
EntityManager::~EntityManager()
{
}

void EntityManager::Initialize() {
	if (instance == nullptr) {
		instance = new EntityManager();
	}
}
EntityManager* EntityManager::getInstance() {
	return instance;
}
void EntityManager::add(Entity* entity) {
	entities->push_back(entity);
}
std::vector<Entity*>* EntityManager::getEntities() {
	return entities;
}
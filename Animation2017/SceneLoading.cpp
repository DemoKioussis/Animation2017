#include "SceneLoading.h"
#include "PhysicsBuilder.h"
#include "EntityManager.h"
SceneLoading* SceneLoading::instance=nullptr;
void SceneLoading::Initialize() {
	if (instance)
		return;
	SceneLoading * sceneLoading = new SceneLoading();
	instance = sceneLoading;
	//instance->setRenderCollisionMeshes();
}
 SceneLoading::SceneLoading() {
	cube = new Mesh("cube.obj", MeshType::VERTICES);
	cylinder = new Mesh("cylinder.obj", MeshType::VERTICES);
	ellipsoid = new Mesh("ellipsoid.obj", MeshType::VERTICES);
	sphere = new Mesh("sphere.obj", MeshType::SPHERE);
	sphereLR = new Mesh("sphereLR.obj", MeshType::VERTICES);
	sphereSky = new Mesh("sphereSky.obj", MeshType::SKYBOX);

	//sphereHD = new Mesh("sphereHD.obj", MeshType::SPHERE);
	RenderEngine::getInstance()->addRenderReference(cube);
	RenderEngine::getInstance()->addRenderReference(cylinder);
	RenderEngine::getInstance()->addRenderReference(ellipsoid);
	RenderEngine::getInstance()->addRenderReference(sphere);
	RenderEngine::getInstance()->addRenderReference(sphereLR);
	RenderEngine::getInstance()->addRenderReference(sphereSky);
	//RenderEngine::getInstance()->addRenderReference(sphereHD);
	CollisionEngine::getInstance()->addMesh(cube);
	CollisionEngine::getInstance()->addMesh(cylinder);
	CollisionEngine::getInstance()->addMesh(ellipsoid);
	CollisionEngine::getInstance()->addMesh(sphere);
	CollisionEngine::getInstance()->addMesh(sphereLR);
	CollisionEngine::getInstance()->addMesh(sphereSky);
	//CollisionEngine::getInstance()->addMesh(sphereHD);

	PhysicsEngine::getInstance()->meshes.push_back(cube);
	PhysicsEngine::getInstance()->meshes.push_back(cylinder);
	PhysicsEngine::getInstance()->meshes.push_back(ellipsoid);
	PhysicsEngine::getInstance()->meshes.push_back(sphere);
	PhysicsEngine::getInstance()->meshes.push_back(sphereLR);
	PhysicsEngine::getInstance()->meshes.push_back(sphereSky);
	//CollisionEngine::getInstance()->addMesh(sphereHD);

	Entity* skybox = new Entity(true, true);
	//skybox->translation = glm::translate(skybox->translation, glm::vec3(0,0,0));

	skybox->scale = glm::scale(skybox->scale, glm::vec3(50.0, 50.0, 50.0));

	RenderComponent *rs = new RenderComponent();
	rs->setMeshID(5);
	skybox->addComponent(rs);
	RenderEngine::getInstance()->addComponent(rs);

}



 void SceneLoading::loadScene(string sceneName) {
	 PhysicsEngine::getInstance()->disable();
	 float timeScale = TimeSystem::getTimeScale();
	 PhysicsEngine::getInstance()->setGravity(glm::vec3(0,-1,0), 0);


	ifstream scene;
	scene.open(sceneName);
	if (!scene) {
		std::cout << "Unable to open scene: " <<sceneName << std::endl;
		//PhysicsEngine::getInstance()->enable();
		TimeSystem::setTimeScale(timeScale);

		return;
	}
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0), 0);

	//float elapsedTime = glfwGetTime();		
	std::vector<Entity*> *Entities;
	Entities = EntityManager::getInstance()->getEntities();
	Entities->clear();
	RenderEngine::Clear();
	PhysicsEngine::Clear();
	CollisionEngine::Clear();

	//skybox
	Entity* skybox = new Entity(true,true);
	//skybox->translation = glm::translate(skybox->translation, glm::vec3(0,0,0));

	skybox->scale = glm::scale(skybox->scale, glm::vec3(50.0, 50.0, 50.0));

	RenderComponent *rs = new RenderComponent();
	rs->setMeshID(5);
	skybox->addComponent(rs);
	RenderEngine::getInstance()->addComponent(rs);

	//PhysicsComponent* ps = new PhysicsComponent();
	//ps->setStatic(true);
	//float massS =1;
	//ps->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(5, glm::mat4(), massS));
	//ps->setMass(massS);
	//skybox->addComponent(ps);
	//PhysicsEngine::getInstance()->addComponent(ps);

	EntityManager::getInstance()->add(skybox);
	string line;
	int currentMeshIndex;
	Entity* e = nullptr;
	bool isStatic;
	glm::vec3 force(0, 0, 0);
	glm::vec3 torque(0,0,0);
	while (getline(scene, line)) {
		splitstring sLine = splitstring(line);
		vector<string> sVec= sLine.split('|');
		if (sVec.size() == 0) {
			continue;
		}
		if (isdigit(sVec[0][0])) {//new entity
			//std::cout << "new ent" << std::endl;
			currentMeshIndex = stoi(sVec[0]); 
			if (e!=nullptr) {//entity loaded all components/add to list
				//std::cout << "pushed" << std::endl;

				EntityManager::getInstance()->add(e);

			}
			Entity* etmp = new Entity(stoi(sVec[1]), currentMeshIndex);
			isStatic = stoi(sVec[1]);
			e = etmp;
			continue;
		}
		else {

			if (sVec[0][0] == '/'&&sVec[0][1] == '/') {
				continue;
			}
			//scene params
			if (sVec[0] == "gravity") {
				if (sVec.size() > 4) {
					PhysicsEngine::getInstance()->setGravity(glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])), stof(sVec[4]));
				}
				else {
					PhysicsEngine::getInstance()->setGravity(glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])), 9.8);
				}
			}
			//load components
			if (sVec[0] == "position") {//position
				//std::cout << "position" << std::endl;
				e->translation =glm::translate(e->translation, glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])));
			}
			if (sVec[0] == "scale") {//scale
				e->scale = glm::scale(e->scale, glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])));
			}
			if (sVec[0] == "rotation") {//rotation
				//e->rotation = glm::eulerAngleYXZ(stof(sVec[2]), stof(sVec[1]), stof(sVec[3]));
				glm::quat tmpQuat;
				tmpQuat.x = stof(sVec[1]);
				tmpQuat.y = stof(sVec[2]);
				tmpQuat.z = stof(sVec[3]);
				tmpQuat.w = stof(sVec[4]);
				e->rotation = glm::toMat4(tmpQuat);
			}
			if (sVec[0] == "render") {//render
				//std::cout << "render" << std::endl;
				RenderComponent *r = new RenderComponent();
				r->setMeshID(currentMeshIndex);

				if (sVec.size() > 1) {
					r->setColor(glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])));
				}
				e->addComponent(r);
				RenderEngine::getInstance()->addComponent(r);
			}
			if (sVec[0] == "physics") {
				//std::cout << "physics" << std::endl;
				PhysicsComponent* p = new PhysicsComponent();
				float mass = stof(sVec[1]);
				p->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(currentMeshIndex, glm::mat4(),mass));
				p->setMass(mass);
				p->setStatic(isStatic);


				//std::cout << e->translation[3][0]<< e->translation[3][1]<< e->translation[3][2] << std::endl;
				force.x = stof(sVec[2]);
				force.y = stof(sVec[3]);
				force.z = stof(sVec[4]);

				//p->setVelocity(force);
				//PhysicsEngine::getInstance()->setVelocity(p);
				PhysicsEngine::getInstance()->addForce(p, force,glm::vec3());
				if (sVec.size() > 7) {
					torque.x = stof(sVec[5]);
					torque.y = stof(sVec[6]);
					torque.z = stof(sVec[7]);
					PhysicsEngine::getInstance()->addTorque(p, torque);

				}
				if (sVec.size() == 9) { //bouciness
					p->setCoeffOfRestitution(stof(sVec[8]));
				}
				else if (sVec.size() == 6) {//backwards compatible bounciness
					p->setCoeffOfRestitution(stof(sVec[5]));
				}
				//if (sVec.size() == 7) {//attractor
				//	if (stoi(sVec[6]) > 0) {
				//		PhysicsEngine::getInstance()->addAttractor(p);
				//	}
				//}

				//if (sVec.size() == 6) { //torque
				//	p->setCoeffOfRestitution(stof(sVec[5]));
				//}
				e->addComponent(p);
				PhysicsEngine::getInstance()->addComponent(p);
			}
			if (sVec[0] == "collider") {
				//std::cout << "collider" << std::endl;
				CollisionComponent* c = new CollisionComponent();
				c->setMeshID(currentMeshIndex);
				
				e->addComponent(c);
				CollisionEngine::getInstance()->addComponent(c);

			}

		}
	}
	if (e != nullptr) {//entity loaded all components/add to list
		//std::cout << "pushed" << std::endl;

		EntityManager::getInstance()->add(e);

	}

	for (Entity* ent : *EntityManager::getInstance()->getEntities())
	{
		ent->transform = ent->translation * ent->rotation * ent->scale;
	}
	//RenderEngine::getInstance()->updateColors();
	CollisionEngine::getInstance()->updateAllBoundingBoxes(); // Can only be called after calculating the unique indices
	
	scene.close();
	//glfwSetTime(elapsedTime);
	TimeSystem::setTimeScale(timeScale);
	TimeSystem::resetTime();
	
	//return *skybox;
}
SceneLoading* SceneLoading::getInstance() {
	return instance;
}
void SceneLoading::saveScene(string sceneName) {
	PhysicsEngine::getInstance()->disable();
	sceneName = sceneName + ".scene";
	
	ofstream scene;
	scene.open(sceneName);

	std::vector<Entity*> *Entities = EntityManager::getInstance()->getEntities();
	glm::vec3 gravity = PhysicsEngine::getInstance()->getGravity();
	//cout << gravity.x << "|" << gravity.y << "|" << gravity.z << endl;
	//if (gravity.x != 0 && gravity.y != 0 && gravity.z != 0) {
	if(glm::length(gravity)>0.01){
		glm::vec3 gravityNorm = glm::normalize(gravity);
		scene << "gravity" << "|" << gravityNorm.x << "|" << gravityNorm.y << "|" << gravityNorm.z << "|"<<glm::length(gravity) << "\n";
	}
	for (int i = 1; i < Entities->size(); i++) {
		Entity* entity = Entities->at(i);
		scene << entity->getMeshID() << "|" << (entity->isStatic()?1:0) << "\n";
		scene << "position" << "|" << entity->translation[3][0] << "|" << entity->translation[3][1] << "|" << entity->translation[3][2] << "\n";
		scene << "scale" << "|" << entity->scale[0][0] << "|" << entity->scale[1][1] << "|" << entity->scale[2][2] << "\n";
		glm::quat tmpQuat =glm::toQuat(entity->rotation);
		scene << "rotation" << "|" << tmpQuat.x << "|" << tmpQuat.y << "|" << tmpQuat.z << "|" << tmpQuat.w << "\n";
		for (int j = 0; j < entity->components.size(); j++) {
			Component* component = entity->components[j];
			std::cout << component->getType() << std::endl;
			RenderComponent* renderComponent = nullptr;
			PhysicsComponent* physicsComponent = nullptr;
			//CollisionComponent* collisionComponent = nullptr;
			float denominator;
			switch (component->getType()) {
				case RENDER_COMPONENT:
					renderComponent = static_cast<RenderComponent*>(component);
					scene << "render" << "|" << renderComponent->getColor().x << "|" << renderComponent->getColor().y << "|" << renderComponent->getColor().z << "\n";
					break;
				case PHYSICS_COMPONENT:
					physicsComponent = static_cast<PhysicsComponent*>(component);
					//cout << physicsComponent->getVeloctiy().x << "|" << physicsComponent->getVeloctiy().y << "|" << physicsComponent->getVeloctiy().z << "|" << endl;

					denominator = (TimeSystem::getPhysicsTimeFactor())/physicsComponent->getMass();
					scene << "physics" << "|" << physicsComponent->getMass() << "|" << physicsComponent->getVeloctiy().x/ denominator << "|" << physicsComponent->getVeloctiy().y / denominator << "|" << physicsComponent->getVeloctiy().z / denominator << "|" << physicsComponent->getAngularVelocity().x / denominator << "|" << physicsComponent->getAngularVelocity().y / denominator << "|" << physicsComponent->getAngularVelocity().z / denominator << "|" << physicsComponent->getCoeffOfRestitution()<<"\n";
					break;
				case COLLISION_COMPONENT:
					scene << "collider" << "\n";
					break;
			}

		}
	}



	scene.close();

}

//SceneLoading::~SceneLoading()
//{
//}

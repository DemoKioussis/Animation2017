#include "SceneLoading.h"
#include "PhysicsBuilder.h"
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
}

 void SceneLoading::loadScene(char * sceneName) {

	char pathfile[100];
	strcpy_s(pathfile, "Scenes/");
	strcat_s(pathfile, sceneName);
	strcpy_s(pathfile, pathfile);

	strcat_s(pathfile, ".scene");
	cout << pathfile << endl;
	ifstream scene;
	scene.open(pathfile);
	if (!scene) {
		std::cout << "Unable to open scene: " << pathfile << std::endl;
		return;
	}
	float timeScale = TimeSystem::getTimeScale();
	TimeSystem::setTimeScale(0);
	//float elapsedTime = glfwGetTime();
	InputManager::Entities->clear();
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

	InputManager::Entities->push_back(skybox);
	string line;
	int currentMeshIndex;
	Entity* e = nullptr;
	bool isStatic;
	glm::vec3 force(0,0,0);
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

				InputManager::Entities->push_back(e);
			}
			Entity* etmp = new Entity(stoi(sVec[1]));
			isStatic = stoi(sVec[1]);
			e = etmp;
			continue;
		}
		else {
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
				PhysicsEngine::getInstance()->addForce(p, force,glm::vec3());
				if (sVec.size() == 6) { //bouciness
					p->setCoeffOfRestitution(stof(sVec[5]));
				}
				if (sVec.size() == 7) {//attractor
					if (stoi(sVec[6]) > 0) {
						PhysicsEngine::getInstance()->addAttractor(p);
					}
				}
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

		InputManager::Entities->push_back(e);
	}

	for (Entity* ent : *InputManager::Entities)
	{
		ent->transform = ent->translation * ent->rotation * ent->scale;
	}
	//RenderEngine::getInstance()->updateColors();
	CollisionEngine::getInstance()->updateAllBoundingBoxes(); // Can only be called after calculating the unique indices
	
	//glfwSetTime(elapsedTime);
	TimeSystem::setTimeScale(timeScale);
	TimeSystem::resetTime();
	//return *skybox;
}
SceneLoading* SceneLoading::getInstance() {
	return instance;
}


//SceneLoading::~SceneLoading()
//{
//}

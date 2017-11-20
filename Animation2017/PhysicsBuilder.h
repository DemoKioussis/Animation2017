#include "Mesh.h"
#include "PhysicsComponent.h"
#include "PhysicsEngine.h"

#include <vector>
using namespace glm;
using namespace std;
class PhysicsBuilder {

public:
	static glm::mat4 getMomentOfInertia(int shape, glm::mat4 scale,float density) {
		const vector<glm::vec3> * uniqueVertices = PhysicsEngine::getInstance()->meshes[0]->getUniqueVerts();
	
		glm::mat4 sum(1.0f);

		for (int i = 0; i < 4;i++) {
			for (int j = 0; j < 4;j++) {
				sum[i][j] = 0;
			}
		}

		for (int i = 0; i < uniqueVertices->size();i++) {

			glm::vec4 r = glm::vec4(uniqueVertices->at(i),0);
			float rr = glm::dot(r, r);
			glm::mat4 I = rr*glm::mat4(1.0f);
			glm::mat4 rrt(1.0f);

			for (int j = 0; j < 3;j++) {
				for (int k = 0; k < 3;k++) {
					rrt[j][k] = r[j] * r[k];
				}
			}
			sum += density*(I - rrt);
		
		}
		sum[3][3] = 1;
		return sum;

	}


};
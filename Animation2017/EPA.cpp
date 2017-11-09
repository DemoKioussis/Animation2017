#include "EPA.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace glm;

EPA::EPA(GJK& gjkWithCollision) : gjk(gjkWithCollision)
{
	vec3& a = gjk.a;
	vec3& b = gjk.b;
	vec3& c = gjk.c;
	vec3& d = gjk.d;
	
	faces.push_back(new Face(a, b, c));
	faces.push_back(new Face(a, d, b));
	faces.push_back(new Face(a, c, d));
	faces.push_back(new Face(c, b, d));
}

glm::vec3 EPA::getPenetrationVector()
{
	const float thresholdToStop = (gjk.meshType1 == MeshType::SPHERE || gjk.meshType2 == MeshType::SPHERE) ? 0.02 : 0.01;	

	for (size_t iterations = 0; iterations < 100; iterations++)
	{		
		float closestFaceDistance;
		Face* closestFace = getClosestFaceToOrigin(closestFaceDistance);

		if (closestFace == nullptr)
		{
			break;
		}

		vec3 normalOfClosest = closestFace->getNormal();
		vec3 supportPoint = gjk.support(normalOfClosest);
		vec3 projection = projectionOnNormal(supportPoint, normalOfClosest);
		float projectionLength = glm::length(projection);
		//cout << ++iterations << ", " << closestFaceDistance << endl;
		if (abs(projectionLength - closestFaceDistance) < thresholdToStop)
		{
			for (size_t i = 0; i < faces.size(); i++)
			{
				delete faces[i];
			}
			return projection;
		}
		else
		{
			extendPolytope(supportPoint);
		}
	}

	return glm::vec3();
}

float EPA::distanceFromFaceToOrigin(Face & face)
{
	return abs(glm::dot(face.getNormal(), face.a));
}

Face* EPA::getClosestFaceToOrigin(float& closestFaceDistance)
{
	const vec3 origin(0,0,0);
	closestFaceDistance = 1e30f;
	Face* closestFace = nullptr;

	for (size_t i = 0; i < faces.size(); i++)
	{
		Face* face = faces[i];
		float currentDistance = distanceFromFaceToOrigin(*face);
		if (currentDistance < closestFaceDistance)
		{
			closestFaceDistance = currentDistance;
			closestFace = face;
		}
	}

	return closestFace;
}

vec3 EPA::projectionOnNormal(glm::vec3 supportPoint, glm::vec3 normal)
{
	return (dot(supportPoint, normal) / dot(normal, normal)) * normal;
}

void EPA::extendPolytope(glm::vec3 extendPoint)
{
	vector<pair<vec3,vec3>> edges;

	for (size_t i = 0; i < faces.size(); i++)
	{
		Face* face = faces[i];

		if (canSeeFaceFromPoint(*face, extendPoint))
		{
			Face& f = *face;
			faces.erase(faces.begin() + i);
			i--;

			addOrRemoveEdge(edges, make_pair(f.a, f.b));
			addOrRemoveEdge(edges, make_pair(f.b, f.c));
			addOrRemoveEdge(edges, make_pair(f.c, f.a));

			delete &f;
		}
	}

	for (pair<vec3, vec3>& edge : edges)
	{
		faces.push_back(new Face(edge.first, edge.second, extendPoint));
	}
}

//bool sameDirection(glm::vec3 & vec1, glm::vec3 & vec2)
//{
//	return dot(vec1, vec2) > 0;
//}

bool EPA::canSeeFaceFromPoint(Face & face, glm::vec3 & point)
{
	if (!gjk.sameDirection(face.getNormal(), point))
	{
		return false;
	}

	vec3& q = point;
	const vec3& p = face.a;
	vec3& normal = face.getNormal();
	vec3 pointProjectedOnPlane = q - dot(q - p, normal) * normal;

	return length(point) > length(pointProjectedOnPlane);
	//return gjk.sameDirection(face.getNormal(), point);
}

void EPA::addOrRemoveEdge(vector<pair<vec3, vec3>>& edges, pair<vec3, vec3>& edge)
{
	static const float treshold = 0.001;
	//vec3 edgeReverse = edge.first - edge.second;

	for (size_t i = 0; i < edges.size(); i++)
	{
		//vec3 e = edges[i].second - edges[i].first;
		vec3 diff1 = edges[i].second - edge.first;
		vec3 diff2 = edges[i].first - edge.second;
		float diffLength1 = length(diff1);
		float diffLength2 = length(diff2);

		if (diffLength1 < treshold && diffLength2 < treshold)
		{
			edges.erase(edges.begin() + i); // Edge already exists
			return;
		}
	}

	// Edge doesn't exist
	edges.push_back(edge);
}

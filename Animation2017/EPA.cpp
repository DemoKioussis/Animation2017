#include "EPA.h"

using namespace std;
using namespace glm;

EPA::EPA(GJK& gjkWithCollision) : gjk(gjkWithCollision)
{
	vec3& a = gjk.a;
	vec3& b = gjk.b;
	vec3& c = gjk.c;
	vec3& d = gjk.d;
	
	faces.push_back(Face(a, b, c));
	faces.push_back(Face(a, d, b));
	faces.push_back(Face(a, c, d));
	faces.push_back(Face(c, b, d));
}

glm::vec3 EPA::getPenetrationVector()
{
	//while (true)
	{
		float closestFaceDistance;
		Face& closestFace = getClosestFaceToOrigin(closestFaceDistance);
		vec3 normalOfClosest = closestFace.getNormal();
		vec3 supportPoint = gjk.support(normalOfClosest);
		float projectionLength = projectionOnNormalLength(supportPoint, normalOfClosest);

		if (abs(projectionLength - closestFaceDistance) < 0.01f)
		{
			return normalOfClosest;
		}
		else
		{
			extendPolytope(supportPoint);
		}
	}

	return glm::vec3();
}

float EPA::distanceFromFaceToPoint(Face & face, glm::vec3 point)
{
	return abs(glm::dot(face.getNormal(), point - face.a));
}

Face & EPA::getClosestFaceToOrigin(float& closestFaceDistance)
{
	const vec3 origin(0,0,0);
	closestFaceDistance = 1e30f;
	Face* closestFace = nullptr;

	for (size_t i = 0; i < faces.size(); i++)
	{
		Face* face = &(faces[i]);
		float currentDistance = distanceFromFaceToPoint(*face, origin);
		if (currentDistance < closestFaceDistance)
		{
			closestFaceDistance = currentDistance;
			closestFace = face;
		}
	}

	return *closestFace;
}

float EPA::projectionOnNormalLength(glm::vec3 supportPoint, glm::vec3 normal)
{
	return length((dot(supportPoint, normal) / dot(normal, normal)) * normal);
}

void EPA::extendPolytope(glm::vec3 extendPoint)
{
	vector<pair<vec3,vec3>> edges;

	for (size_t i = 0; i < faces.size(); i++)
	{
		Face* face = &faces[i];

		if (gjk.sameDirection(face->getNormal(), extendPoint))
		{
			Face f = *face;
			faces.erase(faces.begin() + i);
			i--;

			addOrRemoveEdge(edges, make_pair(f.a,f.b));
			addOrRemoveEdge(edges, make_pair(f.b, f.c));
			addOrRemoveEdge(edges, make_pair(f.c, f.a));
		}
	}

	for (pair<vec3, vec3>& edge : edges)
	{
		faces.push_back(Face(edge.first, edge.second, extendPoint));
	}
}

void EPA::addOrRemoveEdge(vector<pair<vec3, vec3>>& edges, pair<vec3, vec3>& edge)
{
	static const int treshold = 0.001;
	vec3 edgeReverse = edge.first - edge.second;

	for (size_t i = 0; i < edges.size(); i++)
	{
		vec3 e = edges[i].second - edges[i].first;

		if (abs(e.x - edgeReverse.x) < treshold && abs(e.x - edgeReverse.x) < treshold && abs(e.x - edgeReverse.x) < treshold)
		{
			edges.erase(edges.begin() + i); // Edge already exists
			return;
		}
	}

	// Edge doesn't exist
	edges.push_back(edge);
}
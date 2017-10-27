//based on lab and opengl-tutorial.org

#include <vector>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <glm/glm.hpp>

#include "objloader.hpp"

// .OBJ Loader from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

#pragma warning(disable:4996)
//Object loader implementation from tutorial 
bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals, std::vector<glm::vec2> & out_uv)
{
	
	std::vector<unsigned int > vertIndices;
	std::vector< unsigned int > uvIndices;
	std::vector<unsigned int > 	normIndices;
	std::vector< glm::vec3 > tmpVert,tmpNorm;
	std::vector< glm::vec2 > tmpUV;

	printf("Loading OBJ file %s...\n", path);

	FILE * file = fopen(path, "r");

	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	else{
		printf("Object loaded: %s...\n", path);

	}
	int i = 0;
	while( 1 ){
		//std::cout << i;
		//std::cout << "\n";
		//i++;
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tmpVert.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			tmpUV.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tmpNorm.push_back(normal);


		}else if ( strcmp( lineHeader, "f" ) == 0 ){

			std::string vertex1, vertex2, vertex3;
			int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9){
				printf("File can't be read by our simple parser: Try exporting with other options\n");
				std::cout << matches << std::endl;
				return false;
			}
			vertIndices.push_back(vertexIndex[0]);
			vertIndices.push_back(vertexIndex[1]);
			vertIndices.push_back(vertexIndex[2]);

			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);

			normIndices.push_back(normalIndex[0]);
			normIndices.push_back(normalIndex[1]);
			normIndices.push_back(normalIndex[2]);

		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	for (int i = 0; i < vertIndices.size(); i++){
		out_vertices.push_back(tmpVert[vertIndices[i]-1]);
		/*out_vertices.push_back(tmpVert[vertIndices[i]].y);
		out_vertices.push_back(tmpVert[vertIndices[i]].z);*/
	}
	for (int i = 0; i < uvIndices.size(); i++){
		out_uv.push_back(tmpUV[uvIndices[i]-1]);
		//out_uv.push_back(tmpUV[uvIndices[i]].y);
	}
	for (int i = 0; i < normIndices.size(); i++){
		out_normals.push_back(tmpNorm[normIndices[i]-1]);
		//out_normals.push_back(tmpNorm[normIndices[i]].y);
		//out_normals.push_back(tmpNorm[normIndices[i]].z);

	}
	return true;
}


//Refer to slack for blender's exporting settings.
bool loadOBJNoUV(const char * path, std::vector<GLfloat> &out_vertices, std::vector<GLfloat> &out_normals, std::vector<GLuint> &out_indices)
{

	std::vector<unsigned int > vertIndices;
	std::vector<unsigned int > 	normIndices;
	std::vector< glm::vec3 > tmpVert, tmpNorm;

	printf("Loading OBJ file %s...\n", path);

	FILE * file = fopen(path, "r");

	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	else {
		printf("Object loaded: %s...\n", path);

	}
	int i = 0;
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tmpVert.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tmpNorm.push_back(normal);

		}
		else if (strcmp(lineHeader, "f") == 0) {

			std::string vertex1, vertex2, vertex3;
			int vertexIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6) {
				printf("File can't be read by our simple parser: Try exporting with other options\n");
				std::cout << matches << std::endl;
				return false;
			}
			vertIndices.push_back(vertexIndex[0]);
			vertIndices.push_back(vertexIndex[1]);
			vertIndices.push_back(vertexIndex[2]);

			normIndices.push_back(normalIndex[0]);
			normIndices.push_back(normalIndex[1]);
			normIndices.push_back(normalIndex[2]);

			out_indices.push_back(vertexIndex[0]);
			out_indices.push_back(vertexIndex[1]);
			out_indices.push_back(vertexIndex[2]);

		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}


	for (int i = 0; i < normIndices.size(); i++){
		out_normals.push_back(tmpNorm[normIndices[i] - 1].x);
		out_normals.push_back(tmpNorm[normIndices[i] - 1].y);
		out_normals.push_back(tmpNorm[normIndices[i] - 1].z);

	}

	for (int i = 0; i < vertIndices.size(); i++){
		out_vertices.push_back(tmpVert[vertIndices[i] - 1].x);
		out_vertices.push_back(tmpVert[vertIndices[i] - 1].y);
		out_vertices.push_back(tmpVert[vertIndices[i] - 1].z);
	}



	return true;
}
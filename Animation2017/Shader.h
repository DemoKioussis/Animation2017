#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
	Shader class, copied from learnopengl.org, however I added some extra functions for more control over uniforms
*/
class Shader
{
public:
	unsigned int ID;

	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
		
	}
	// generic utility uniform functions by name
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool &value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setBool(const GLuint &loc, const bool &value) const
	{
		glUniform1i(loc, (int)value);
	}

	//INT
	void setInt(const std::string &name, const int &value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt(GLuint &loc, const int &value) const
	{
		glUniform1i(loc, value);
	}

	//FLOAT
	void setFloat(const std::string &name, const float &value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const GLuint &loc, const float &value) const
	{
		glUniform1f(loc, value);
	}

	//VEC3
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}
	void setVec3(const GLuint &loc, const glm::vec3 &value) const
	{
		glUniform3f(loc, value.x, value.y, value.z);
	}

	//VEC4
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
	}
	void setVec4(const GLuint &loc, const glm::vec4 &value) const
	{
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	//MAT
	void setMat4(const std::string &name, const glm::mat4 &value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void setMat4(const GLuint &loc, const glm::mat4 &value) const
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}
	static void skyboxFlag(int isRenderingSkybox) {
		glUniform1i(isRenderSkyboxLoc, isRenderingSkybox);
	}
	static GLuint isRenderSkyboxLoc;
private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------

	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};

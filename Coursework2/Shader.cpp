#include "Shader.h"

#include <iostream>
#include <fstream>

#include <glad/glad.h>

Shader::Shader()
{

}
void Shader::init(const char* vertexPath, const char* fragmentPath)
{
	programID = glCreateProgram();
	try
	{
		CreateAndLoadShaderToProgram(GL_VERTEX_SHADER, vertexPath);
		CreateAndLoadShaderToProgram(GL_FRAGMENT_SHADER, fragmentPath);
	}
	catch (Exception exc)
	{
		std::cerr << exc.message << std::endl;
	}

	glLinkProgram(programID);

	{
		int success;
		char infoLog[512];

		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			std::cerr << infoLog << std::endl;
		}
	}
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	programID = glCreateProgram();
	try
	{
		CreateAndLoadShaderToProgram(GL_VERTEX_SHADER, vertexPath);
		CreateAndLoadShaderToProgram(GL_FRAGMENT_SHADER, fragmentPath);
	}
	catch (Exception exc)
	{
		std::cerr << exc.message << std::endl;
	}

	glLinkProgram(programID);

	{
		int success;
		char infoLog[512];

		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			std::cerr << infoLog << std::endl;
		}
	}
}

void Shader::use() const
{
	glUseProgram(programID);
}

const unsigned Shader::GetProgramID() const
{
	return programID;
}

void Shader::LoadShaderSourceFromFile(unsigned shaderID, const char* fileName) const
{
	std::ifstream ShaderSource(fileName);
	if (ShaderSource.fail())
		throw Exception("Failed to open shader file!", 4);
	const std::string SourceString((std::istreambuf_iterator<char>(ShaderSource)), std::istreambuf_iterator<char>());
	const char* str = SourceString.c_str();
	glShaderSource(shaderID, 1, &str, nullptr);
}

void Shader::GetShaderCompilationStatus(unsigned shaderID) const
{
	int success;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		throw Exception(infoLog, 5);
	}
	else
		std::cerr << "Shader compiled successfully!\n";
}

void Shader::CreateAndLoadShaderToProgram(unsigned shaderType, const char* fileName) const
{
	unsigned ShaderID = glCreateShader(shaderType);
	LoadShaderSourceFromFile(ShaderID, fileName);
	glCompileShader(ShaderID);
	GetShaderCompilationStatus(ShaderID);
	glAttachShader(programID, ShaderID);
	glDeleteShader(ShaderID);
}

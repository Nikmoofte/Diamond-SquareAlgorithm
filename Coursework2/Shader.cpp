#include "Shader.h"

#include <Windows.h>
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
		MessageBox(NULL, exc.message, L"ERROR", MB_OK | MB_ICONERROR);
	}

	glLinkProgram(programID);

	{
		int success;
		char infoLog[512];

		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			MessageBoxA(NULL, infoLog, "ERROR", MB_OK | MB_ICONERROR); 
		}
	}
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	init(vertexPath, fragmentPath);
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
		throw Exception(L"Failed to open shader file!", 4);
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
		MessageBoxA(NULL, infoLog, "ERROR", MB_OK | MB_ICONERROR);
	}
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

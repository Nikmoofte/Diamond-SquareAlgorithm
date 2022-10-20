#pragma once


class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader();
	void init(const char* vertexPath, const char* fragmentPath);
	void use() const;
	const unsigned GetProgramID() const;


private:
	struct Exception
	{
		Exception(const char* msg, int num) : message(msg), number(num) {}
		const char* message;
		int number;
	};

	unsigned programID;

	void LoadShaderSourceFromFile(unsigned shaderID, const char* fileName) const;

	void GetShaderCompilationStatus(unsigned shaderID) const;

	void CreateAndLoadShaderToProgram(unsigned shaderType, const char* fileName) const;

};


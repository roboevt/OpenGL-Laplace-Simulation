#pragma once
#include <string>

class Shader {
public:
	unsigned int shader;

	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	Shader();
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	int create(const std::string& vertexSource, const std::string& fragmentSource);

	operator int() const { return shader; }

private:
	ShaderProgramSource parseGLSL(const std::string& vertexPath, const std::string& fragmentPath);
	static ShaderProgramSource parseShader(const std::string& filePath);
	unsigned int createShader(const ShaderProgramSource& source);
	unsigned int compileShader(unsigned int type, const std::string& source);
};


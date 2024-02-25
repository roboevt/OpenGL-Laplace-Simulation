#include "Shader.h"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : shader(0) { }

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    ShaderProgramSource source = parseGLSL(vertexPath, fragmentPath);
    if (source.VertexSource == "" || source.FragmentSource == "") shader = 0;
    else shader = createShader(source);
}

int Shader::create(const std::string& vertexSource, const std::string& fragmentSource) {
    shader = createShader({ vertexSource, fragmentSource });
    return shader;
}

Shader::ShaderProgramSource Shader::parseGLSL(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream stream(vertexPath);
    if (stream.is_open()) {
        std::string line;
        std::stringstream ss[2];
        while (getline(stream, line)) { ss[0] << line << '\n'; }
        stream.close();
        stream.open(fragmentPath);
        if (stream.is_open()) {
            while (getline(stream, line)) { ss[1] << line << '\n'; }
            return { ss[0].str(), ss[1].str() };
        }
    }
    std::cout << "Could not load shader. Vertex: " << vertexPath << " Fragment: " << fragmentPath << std::endl;
    return { "", ""};
}

Shader::ShaderProgramSource Shader::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            if (type != ShaderType::NONE) ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::createShader(const ShaderProgramSource& source) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

    if (vs == 0 || fs == 0) return 0;

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));  // dynamic stack allocation
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" <<
            message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

Shader::~Shader() {
    //glDeleteProgram(shader);
}
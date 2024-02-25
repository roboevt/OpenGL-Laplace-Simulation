#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Engine.h"
#include "shaderBackup.h"

constexpr auto ROOT_DIR = "/dev/566/Laplace/Laplace/";

int main() {
    Engine engine = Engine();

    const std::string vertexCode = ROOT_DIR + std::string("res/shaders/vertex.glsl");
    const std::string laplaceFragmentCode = ROOT_DIR + std::string("res/shaders/laplace.frag");
    const std::string screenFragmentCode = ROOT_DIR + std::string("res/shaders/screen.frag");

    Shader laplaceShader = Shader(vertexCode, laplaceFragmentCode);
    Shader screenShader = Shader(vertexCode, screenFragmentCode);

    // Load backup shaders if files are not found
    if (!laplaceShader) laplaceShader.create(vertexBackup, laplaceFragmentBackup);
    if (!screenShader) screenShader.create(vertexBackup, screenFragmentBackup);
    
    engine.setShaders(laplaceShader, screenShader);

    engine.setWindowSize(1920, 1080);

    while (!engine.draw()) {
        engine.update();
    }
    return 0;
}
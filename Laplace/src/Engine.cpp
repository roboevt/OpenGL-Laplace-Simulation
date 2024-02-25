#include "Engine.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace {  // Helper functions that do not need (or can't have in case of callback) access to
             // class members

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                GLsizei length, const GLchar* message, const void* userParam) {
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->setWindowSize(width, height);
}


// Called when a key is pressed, with debouncing.
// Used for actions that do not need to be repeated (like toggling fullscreen)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    static bool isFullscreen = false;
    static int lastWindowedWidth, lastWindowedHeight;
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        isFullscreen = !isFullscreen;
        if (isFullscreen) {
            // Switch to fullscreen mode
            glfwGetWindowSize(window, &lastWindowedWidth, &lastWindowedHeight);

            auto currentMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(currentMonitor);
            glfwSetWindowMonitor(window, currentMonitor, 0, 0, mode->width, mode->height,
                                 mode->refreshRate);
        } else {
            // Switch back to windowed mode
            glfwSetWindowMonitor(window, nullptr, 100, 100, lastWindowedWidth, lastWindowedHeight,
                                 GLFW_DONT_CARE);
        }
    }
}

void createFullscreenQuad() {
    float positions[] = {-1, -1, 1,  -1, 1,  1,

                         1,  1,  -1, 1,  -1, -1};

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
}
}  // namespace

Engine::Engine(int width, int height, int samples)
    : windowWidth(width),
      windowHeight(height),
      lastWindowedWidth(0),
      lastWindowedHeight(0),
      samples(samples),
      lastUpdateTime(std::chrono::high_resolution_clock::now()),
      lastFrameTime(std::chrono::high_resolution_clock::now()),
      startTime(std::chrono::high_resolution_clock::now()),
      frames(0),
      lastFrameDuration(0) {

    if (!glfwInit()) throw "GLFW failed to initialize";
    window = glfwCreateWindow(windowWidth, windowHeight, "Laplace Calculator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw "Failed to create Window";
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // request no vsync
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (glewInit() != GLEW_OK) throw "GLEW failed to initialize";
    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);  // debug messages
    glDebugMessageCallback(MessageCallback, 0);

    createFullscreenQuad();
    createFBO();
}

int Engine::createFBO() {
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glCreateTextures(GL_TEXTURE_2D, 1, &bufferTexture);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);

    std::vector<uint8_t> testTextureData(windowWidth * windowHeight * 3, 0xff/2);  // Initialize with default color

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, testTextureData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating framebuffer with status: "
                  << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        return 1;
    }

    return 0;
}

void Engine::setShaders(Shader laplaceShader, Shader screenShader) {
    this->laplaceShader = laplaceShader;
    this->screenShader = screenShader;
}

int Engine::draw() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(laplaceShader);
    glDrawArrays(GL_TRIANGLES, 0, 6);  // draw rendered image to buffer texture

    glUseProgram(screenShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);  // display buffer to screen

    glfwSwapBuffers(window);
    glfwPollEvents();
    return glfwWindowShouldClose(window);
}

void Engine::clearBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
}


void Engine::updateFPS() {
    frames++;
    auto now = std::chrono::high_resolution_clock::now();
    lastFrameDuration = std::chrono::duration<float>(now - lastFrameTime).count();
    lastFrameTime = now;

    auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
    if (elapsedMs >= 1000) {
        float fps = (frames - lastFrames) / ((float)elapsedMs / 1000);
        lastFrames = frames;
        lastUpdateTime = now;

        // std::cout << "fps: " << fps << std::endl;
        std::string title = "Ray Tracer, " + std::to_string((int)fps) + "fps";
        glfwSetWindowTitle(window, title.c_str());
    }
}

void Engine::update() {
    updateFPS();
}

float Engine::getLastFrametime() { return lastFrameDuration; }

float Engine::getTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - startTime).count();
}

int Engine::getShader() { return laplaceShader; }

void Engine::setWindowSize(int width, int height) {
    this->windowWidth = width;
    this->windowHeight = height;
    glViewport(0, 0, width, height);

    glUseProgram(laplaceShader);
    int loc = glGetUniformLocation(laplaceShader, "uResolution");
    glUniform2f(loc, (float)windowWidth, (float)windowHeight);
    glUseProgram(screenShader);
    loc = glGetUniformLocation(screenShader, "uResolution");
    glUniform2f(loc, (float)windowWidth, (float)windowHeight);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);

    draw();
}

Engine::~Engine() { glfwTerminate(); }
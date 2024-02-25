#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <chrono>

#include "Shader.h"


class Engine {
private:
	GLFWwindow* window;
	int windowWidth, windowHeight, lastWindowedWidth, lastWindowedHeight;
	int samples;

	unsigned int fbo, bufferTexture;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime, lastFrameTime, startTime;
	
	unsigned int frames, lastFrames;
	float lastFrameDuration;
	
	Shader laplaceShader, screenShader;

	int createFBO();

	void updateFPS();

	void clearBuffer();
public:
	Engine(int width = 1920, int height = 1080, int samples = 16);
	void setShaders(Shader laplaceShader, Shader screenShader);
	~Engine();
	int draw();
	void update();
	float getLastFrametime();
	float getTime();
	int getShader();

	void setWindowSize(int width, int height);
};


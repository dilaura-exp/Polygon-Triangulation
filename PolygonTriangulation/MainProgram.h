#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>

#include "PolygonTriangulation.h"

enum class ProgramState{RUNNING, EXIT};

class PolygonTriangulation;

class MainProgram {
public:
	MainProgram();
	~MainProgram();

	void run();
	void draw();

private:
	SDL_Window* window;
	int screenWidth;
	int screenHeight;
	ProgramState programState;

	float time;

	Uint32 oldTime, currentTime;
	float deltaTime;

	void initSystems();
	void programLoop();
	void processInput();
	void drawProgram();

	PolygonTriangulation *polygonTriangulation;
};


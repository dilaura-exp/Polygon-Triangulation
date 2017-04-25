#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <vector>

#include <DCEL.h>
#include <queue>

enum class ProgramState{RUNNING, EXIT};

class PolygonTriangulation {
public:
	PolygonTriangulation();
	~PolygonTriangulation();

	void run();

private:
	SDL_Window* window;
	int screenWidth;
	int screenHeight;
	ProgramState programState;

	float time;

	Uint32 oldTime, currentTime;
	float deltaTime;

	void initSystems();
	void gameLoop();
	void processInput();
	void drawGame();

	std::priority_queue<Vertex> vertices;

	void makeMonotone();
};


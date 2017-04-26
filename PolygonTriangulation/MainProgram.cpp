#include <iostream>
#include <string>

#include "MainProgram.h"
#include "Errors.h"
#include "ImageLoader.h"

MainProgram::MainProgram() {
	screenWidth = 800;
	screenHeight = 600;
	time = 0.0f;
	window = nullptr;
	programState = ProgramState::RUNNING;
	currentTime = SDL_GetTicks();
	deltaTime = 0;

	polygonTriangulation = new PolygonTriangulation();
}

MainProgram::~MainProgram() {
	
}

void MainProgram::run() {
	initSystems();
	polygonTriangulation->initPolygon();
	polygonTriangulation->makeMonotone();
	gameLoop();
}

void MainProgram::initSystems() {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Kecerdasan Komputasional", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		fatalError("SDL Window could not be created.");
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr) {
		fatalError("SDL_GL context could not be created.");
	}

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		fatalError("Could not initialize glew.");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}


void MainProgram::gameLoop() {
	while (programState != ProgramState::EXIT) {
		oldTime = currentTime;
		currentTime = SDL_GetTicks();
		deltaTime = (currentTime - oldTime) / 1000.0f;

		processInput();
		time += 10*deltaTime;
		drawGame();
	}
}

void MainProgram::processInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
			case SDL_QUIT:
				programState = ProgramState::EXIT;
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_KEYDOWN:
				switch (evnt.key.keysym.sym) {
					case SDLK_SPACE:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
		}
	}
}

void MainProgram::drawGame() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	//glTranslatef(0, 0, 0);
	polygonTriangulation->draw();

	SDL_GL_SwapWindow(window);
}

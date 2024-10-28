
#ifndef SIMULATIONS_H
#define SIMULATIONS_H

// Must be before any SFML or glfw or glm or glew files                        
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx/string_cast.hpp>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace pe {

	void runWreckingBall();

	void runStaircase();

	void runBallPit();

	void runRagdoll();

	void runFlag();

	void runReflection();

	void runMirrors();

	void runClothSimulation();

	void runVideoGameLevel();
}

#endif
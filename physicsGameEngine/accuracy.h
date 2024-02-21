// Header file that defines the precision of the program in one place

#ifndef ACCURACY_H
#define ACCURACY_H

// For SFML and GLEW static version (no dlls)
#define SFML_STATIC
#define GLEW_STATIC

// Can't have both Glew and Glad
// Glew has to be included first
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/gl.h>

#include <cfloat>
#include <limits>
#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define realSqrt sqrtf
#define realPow powf
#define realAbs fabs
#define REAL_MAX DBL_MAX

namespace pe {
	typedef float real;
	constexpr real PI = 3.141592f;

	// Enum
	enum class Order {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};
}

#endif

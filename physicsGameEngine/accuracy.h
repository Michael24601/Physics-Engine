// Header file that defines the precision of the program in one place

#ifndef ACCURACY_H
#define ACCURACY_H

// For GLEW static version (no dlls) and stb library
// Note that they also need to be included in the preprocessor definitions
#define GLEW_STATIC
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Can't have both Glew and Glad
// Glew has to be included first
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/gl.h>

#include <cfloat>
#include <limits>
#include <iostream>
#include <vector>

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

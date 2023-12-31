// Header file that defines the precision of the program in one place

#ifndef ACCURACY_H
#define ACCURACY_H

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
	enum class order {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};
}

#endif

// Header file that defines the precision of the program in one place

#ifndef ACCURACY_H
#define ACCURACY_H

#include <cfloat>

#define realSqrt sqrtf
#define realPow powf
#define realAbs fabs
#define REAL_MAX DBL_MAX

namespace pe {
	typedef float real;
	constexpr real PI = 3.141592;
}

#endif

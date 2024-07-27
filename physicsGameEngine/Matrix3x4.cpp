
#include "matrix3x4.h"

using namespace pe;

/*
	The identity of the transform matrix is that which does not change
	the position of a point. This means the translation compoenent (last
	column) must be all 0s, as it is added in the transformation process.
	The rotation component, which multiplies transformed points, is the
	identity matrix I_3, which does not affect mutliplied points.
	Another explanation is that the 3 by 4 matrix is actually a homogeneous
	4 by 4matrix, with the last row being (0, 0, 0, 1). In this case,
	adding the last row to this identity gives us the identity matrix
	I_4.
*/
const Matrix3x4 Matrix3x4::IDENTITY(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0
);
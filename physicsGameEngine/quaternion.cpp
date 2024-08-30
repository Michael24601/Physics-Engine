
#include "quaternion.h"
#include "matrix3x3.h"

using namespace pe;


const Quaternion Quaternion::IDENTITY(1, 0, 0, 0);


Quaternion::Quaternion(const Matrix3x3& m){
	real trace = m.data[0] + m.data[4] + m.data[8];

	if (trace > 0) {
		real s = 0.5 / sqrt(trace + 1.0);
		r = 0.25 / s;
		i = (m.data[7] - m.data[5]) * s;
		j = (m.data[2] - m.data[6]) * s;
		k = (m.data[3] - m.data[1]) * s;
	}
	else {
		if (m.data[0] > m.data[4] && m.data[0] > m.data[8]) {
			real s = 2.0 * sqrt(1.0 + m.data[0] - m.data[4] - m.data[8]);
			r = (m.data[7] - m.data[5]) / s;
			i = 0.25 * s;
			j = (m.data[1] + m.data[3]) / s;
			k = (m.data[2] + m.data[6]) / s;
		}
		else if (m.data[4] > m.data[8]) {
			real s = 2.0 * sqrt(1.0 + m.data[4] - m.data[0] - m.data[8]);
			r = (m.data[2] - m.data[6]) / s;
			i = (m.data[1] + m.data[3]) / s;
			j = 0.25 * s;
			k = (m.data[5] + m.data[7]) / s;
		}
		else {
			real s = 2.0 * sqrt(1.0 + m.data[8] - m.data[0] - m.data[4]);
			r = (m.data[3] - m.data[1]) / s;
			i = (m.data[2] + m.data[6]) / s;
			j = (m.data[5] + m.data[7]) / s;
			k = 0.25 * s;
		}
	}
}
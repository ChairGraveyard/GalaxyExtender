#pragma once
#include "Vector.h"

class Transform {
	float matrix[3][4];

public:
	Vector getPosition_p() const {
		return Vector(matrix[0][3], matrix[1][3], matrix[2][3]);
	}

	void Transform::setPosition_p(const Vector &vec) {
		matrix[0][3] = vec.getX();
		matrix[1][3] = vec.getY();
		matrix[2][3] = vec.getZ();
	}

};
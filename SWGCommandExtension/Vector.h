#pragma once

#include "soewrappers.h"

class Vector {
protected:
	float values[3];

public:
	/*static const Vector ZERO;

	//Axis Representations
	static const Vector UNIT_X;
	static const Vector UNIT_Y;
	static const Vector UNIT_Z;*/

public:
	inline Vector() {
		memset(values, 0, sizeof(values));
	}

	Vector(const Vector& vec) {
		memcpy(values, vec.values, sizeof(values));
	}

	inline Vector(const float fx, const float fy, const float fz) {
		values[0] = fx;
		values[1] = fy;
		values[2] = fz;
	}

	/**
	* Converts a 3 dimensional float array into a Vector.
	*/
	inline explicit Vector(const float coord[3]) {
		memcpy(values, coord, sizeof(values));
	}

	/**
	* Converts a 3 dimensional int array into a Vector.
	*/
	inline explicit Vector(const int coord[3]) {
		values[0] = (float)coord[0];
		values[1] = (float)coord[1];
		values[2] = (float)coord[2];
	}

	/**
	* Converts a float scalar to a Vector.
	*/
	inline explicit Vector(const float scalar) {
		values[0] = scalar;
		values[1] = scalar;
		values[2] = scalar;
	}

	virtual ~Vector() {
	}

public:
	/**
	* Returns the exact length of the vector. Should be used sparingly as it
	* uses much CPU power. Use squaredLength for comparing lengths.
	*/
	inline float length() const {
		return std::sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
	}

	/**
	* Returns the length before being squared. Good for comparing lengths.
	*/
	inline float squaredLength() const {
		return (values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
	}

	/**
	* Returns the real distance to another vector. Should not be used for comparisons.
	* See squaredDistanceTo(.
	* \param v The vector to get the distance to.
	* \return Returns the float representation of the distance between the two vectors.
	*/
	inline float distanceTo(const Vector& v) const {
		return (*this - v).length();
	}

	/**
	* Good for comparisons of distance. Saves the cpu usage needed for sqrt.
	* \param v The vector to get the distance to.
	* \return Returns the float representation of the distance between two points prior to application of a square root.
	*/
	inline float squaredDistanceTo(const Vector& v) const {
		return (*this - v).squaredLength();
	}

	/**
	* This method returns the midpoint between this vector and another vector.
	* \param v The vector to find a midpoint in conjunction with.
	* \return Returns a vector half way in between this vector and the passed in vector.
	*/
	inline Vector midPoint(const Vector& v) const {
		return Vector((values[0] + v.values[0]) * 0.5f, (values[1] + v.values[1]) * 0.5f, (values[2] + v.values[2]) * 0.5f);
	}

	/**
	* This method is used to calculate the angle between two vectors. If both
	* are unit vectors then the product is the cosine of the angle; otherwise,
	* the dot product must be divided by the product of the length
	*/
	inline float dotProduct(const Vector& v) const {
		return (values[0] * v.values[0] + values[1] * v.values[1] + values[2] * v.values[2]);
	}

	inline float product() {
		return dotProduct(Vector(values));
	}

	/**
	* The cross product returns the vector which is perpendicular to this vector
	* and the vector passed in. For example, if you have two vectors in the same
	* geometric plane, then the vector would be on the plane perpendicular to that
	* plane, and the vector perpendicular to the two vectors. Imagine a 3D o
	*/
	inline Vector crossProduct(const Vector& v) const {
		return Vector(values[1] * v.values[2] - values[2] * v.values[1], values[2] * v.values[0] - values[0] * v.values[2], values[0] * v.values[1] - values[1] * v.values[0]);
	}

	/**
	* Returns the string representation of the vector in (x, y, z) format.
	*/
	
	inline float operator [] (uint32_t index) const {
		if (index > 2)
			throw std::exception("out of bounds");

		return values[index];
	}

	inline float& operator [] (uint32_t index) {
		if (index > 2)
			throw std::exception("out of bounds");

		return values[index];
	}

	inline float* toFloatArray() {
		return values;
	}

	inline const float* toFloatArray() const {
		return values;
	}

	inline Vector& operator = (const Vector& v) {
		values[0] = v.values[0];
		values[1] = v.values[1];
		values[2] = v.values[2];

		return *this;
	}

	inline Vector& operator = (const float scalar) {
		values[0] = scalar;
		values[1] = scalar;
		values[2] = scalar;

		return *this;
	}

	inline Vector operator + (const Vector& v) const {
		return Vector(values[0] + v.values[0], values[1] + v.values[1], values[2] + v.values[2]);
	}

	inline Vector operator - (const Vector& v) const {
		return Vector(values[0] - v.values[0], values[1] - v.values[1], values[2] - v.values[2]);
	}

	inline Vector operator * (const Vector& v) const {
		return Vector(values[0] * v.values[0], values[1] * v.values[1], values[2] * v.values[2]);
	}

	/*Vector operator*(const Matrix4& mat) const {
		return Vector(values[0] * mat[0][0] + values[1] * mat[1][0] + values[2] * mat[2][0] + mat[3][0],
			values[0] * mat[0][1] + values[1] * mat[1][1] + values[2] * mat[2][1] + mat[3][1],
			values[0] * mat[0][2] + values[1] * mat[1][2] + values[2] * mat[2][2] + mat[3][2]);
	}*/

	inline Vector operator / (const Vector& v) const {
		return Vector(values[0] / v.values[0], values[1] / v.values[1], values[2] / v.values[2]);
	}

	inline Vector& operator += (const Vector& v) {
		values[0] += v.values[0];
		values[1] += v.values[1];
		values[2] += v.values[2];

		return *this;
	}

	inline Vector& operator -= (const Vector& v) {
		values[0] -= v.values[0];
		values[1] -= v.values[1];
		values[2] -= v.values[2];

		return *this;
	}

	inline Vector& operator *= (const Vector& v) {
		values[0] *= v.values[0];
		values[1] *= v.values[1];
		values[2] *= v.values[2];

		return *this;
	}

	inline Vector& operator /= (const Vector& v) {
		values[0] /= v.values[0];
		values[1] /= v.values[1];
		values[2] /= v.values[2];

		return *this;
	}

	inline friend Vector operator + (const Vector& v, const float scalar) {
		return Vector(v.values[0] + scalar, v.values[1] + scalar, v.values[2] + scalar);
	}

	inline friend Vector operator + (const float scalar, const Vector& v) {
		return Vector(scalar + v.values[0], scalar + v.values[1], scalar + v.values[2]);
	}

	inline friend Vector operator - (const Vector& v, const float scalar) {
		return Vector(v.values[0] - scalar, v.values[1] - scalar, v.values[2] - scalar);
	}

	inline friend Vector operator - (const float scalar, const Vector& v) {
		return Vector(scalar - v.values[0], scalar - v.values[1], scalar - v.values[2]);
	}

	inline friend Vector operator * (const Vector& v, const float scalar) {
		return Vector(v.values[0] * scalar, v.values[1] * scalar, v.values[2] * scalar);
	}

	inline friend Vector operator * (const float scalar, const Vector& v) {
		return Vector(scalar * v.values[0], scalar * v.values[1], scalar * v.values[2]);
	}

	inline friend Vector operator / (const Vector& v, const float scalar) {
		if (scalar == 0.0f)
			throw std::exception("divide by 0");

		return Vector(v.values[0] / scalar, v.values[1] / scalar, v.values[2] / scalar);
	}

	inline friend Vector operator / (const float scalar, const Vector& v) {
		return Vector(scalar / v.values[0], scalar / v.values[1], scalar / v.values[2]);
	}

	//Boolean operators
	inline bool operator == (const Vector& v) const {
		return (values[0] == v.values[0] && values[1] == v.values[1] && values[2] == v.values[2]);
	}

	inline bool operator != (const Vector& v) const {
		return (values[0] != v.values[0] || values[1] != v.values[1] || values[2] != v.values[2]);
	}

	inline bool operator < (const Vector& v) const {
		if (values[0] < v.values[0] && values[1] < v.values[1] && values[2] < v.values[2])
			return true;

		return false;
	}

	inline bool operator > (const Vector& v) const {
		if (values[0] > v.values[0] && values[1] > v.values[1] && values[2] > v.values[2])
			return true;

		return false;
	}

	Vector getMin(const Vector& vec) const {
		Vector minVector;

		if (values[0] < vec.values[0])
			minVector.setX(values[0]);
		else
			minVector.setX(vec.values[0]);

		if (values[1] < vec.values[1])
			minVector.setY(values[1]);
		else
			minVector.setY(vec.values[1]);

		if (values[2] < vec.values[2])
			minVector.setZ(values[2]);
		else
			minVector.setZ(vec.values[2]);

		return minVector;
	}

	inline Vector getMax(const Vector& vec) const {
		Vector maxVector;

		if (values[0] > vec.values[0])
			maxVector.setX(values[0]);
		else
			maxVector.setX(vec.values[0]);

		if (values[1] > vec.values[1])
			maxVector.setY(values[1]);
		else
			maxVector.setY(vec.values[1]);

		if (values[2] > vec.values[2])
			maxVector.setZ(values[2]);
		else
			maxVector.setZ(vec.values[2]);

		return maxVector;
	}

	inline float normalize() {
		float magnitude = length();

		values[0] /= magnitude;
		values[1] /= magnitude;
		values[2] /= magnitude;

		return magnitude;
	}

	//Getters
	inline float getX() const {
		return values[0];
	}

	inline float getY() const {
		return values[1];
	}

	inline float getZ() const {
		return values[2];
	}

	//Setters
	inline void setX(float xv) {
		values[0] = xv;
	}

	inline void setY(float yv) {
		values[1] = yv;
	}

	inline void setZ(float zv) {
		values[2] = zv;
	}

	inline void set(float x, float z, float y) {
		this->values[0] = x;
		this->values[1] = y;
		this->values[2] = z;
	}

	friend class Quaternion;
};

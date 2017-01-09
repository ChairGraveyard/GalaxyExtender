#pragma once

#include <math.h>

#define POINT_EPSILON (1e-4)

//--------------------------------------
static void m_point3F_normalize_f_C(float *p, float val)
{
	float factor = val / sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
	p[0] *= factor;
	p[1] *= factor;
	p[2] *= factor;
}

static void m_point3F_normalize_C(float *p)
{
	float squared = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
	// This can happen in Container::castRay -> ForceFieldBare::castRay
	//AssertFatal(squared != 0.0, "Error, zero length vector normalized!");
	if (squared != 0.0f) {
		float factor = 1.0f / sqrt(squared);
		p[0] *= factor;
		p[1] *= factor;
		p[2] *= factor;
	}
	else {
		p[0] = 0.0f;
		p[1] = 0.0f;
		p[2] = 1.0f;
	}
}

static void m_point3F_interpolate_C(const float *from, const float *to, float factor, float *result)
{
	float inverse = 1.0f - factor;
	result[0] = from[0] * inverse + to[0] * factor;
	result[1] = from[1] * inverse + to[1] * factor;
	result[2] = from[2] * inverse + to[2] * factor;
}

class Point3F
{
	//-------------------------------------- Public data
public:
	float x;
	float y;
	float z;

public:
	Point3F();
	Point3F(const Point3F&);
	Point3F(float _x, float _y, float _z);
	explicit Point3F(float xyz);

	//-------------------------------------- Non-math mutators and misc functions
public:
	void set(float xyz);
	void set(float _x, float _y, float _z);
	void set(const Point3F&);

	void setMin(const Point3F&);
	void setMax(const Point3F&);

	void interpolate(const Point3F&, const Point3F&, float);
	void zero();

	/// Returns the smallest absolute value.
	float least() const;

	/// Returns the greatest absolute value.
	float most() const;

	operator float*() { return &x; }
	operator const float*() const { return &x; }

	//-------------------------------------- Queries
public:
	bool  isZero() const;
	bool  isUnitLength() const;
	float   len()    const;
	float   lenSquared() const;
	float   magnitudeSafe() const;
	bool  equal(const Point3F &compare, float epsilon = POINT_EPSILON) const;
	unsigned int getLeastComponentIndex() const;
	unsigned int getGreatestComponentIndex() const;

	//-------------------------------------- Mathematical mutators
public:
	void neg();
	void normalize();
	void normalizeSafe();
	void normalize(float val);
	void convolve(const Point3F&);
	void convolveInverse(const Point3F&);

	//-------------------------------------- Overloaded operators
public:
	// Comparison operators
	bool operator==(const Point3F&) const;
	bool operator!=(const Point3F&) const;

	// Arithmetic w/ other points
	Point3F  operator+(const Point3F&) const;
	Point3F  operator-(const Point3F&) const;
	Point3F& operator+=(const Point3F&);
	Point3F& operator-=(const Point3F&);

	// Arithmetic w/ scalars
	Point3F  operator*(float) const;
	Point3F  operator/(float) const;
	Point3F& operator*=(float);
	Point3F& operator/=(float);

	Point3F  operator*(const Point3F&) const;
	Point3F& operator*=(const Point3F&);
	Point3F  operator/(const Point3F&) const;
	Point3F& operator/=(const Point3F&);

	// Unary operators
	Point3F operator-() const;

	//-------------------------------------- Public static constants
public:
	const static Point3F One;
	const static Point3F Zero;
	const static Point3F Max;
	const static Point3F Min;
	const static Point3F UnitX;
	const static Point3F UnitY;
	const static Point3F UnitZ;
};

typedef Point3F VectorF;
typedef Point3F EulerF;

//------------------------------------------------------------------------------
//-------------------------------------- Point3F
//
inline Point3F::Point3F()
#if defined(TORQUE_OS_LINUX)
	: x(0.f), y(0.f), z(0.f)
#endif
{
	// Uninitialized points are definitely a problem.
	// Enable the following code to see how often they crop up.
#ifdef DEBUG_MATH
	*(U32 *)&x = 0x7FFFFFFA;
	*(U32 *)&y = 0x7FFFFFFB;
	*(U32 *)&z = 0x7FFFFFFC;
#endif
}


inline Point3F::Point3F(const Point3F& _copy)
	: x(_copy.x), y(_copy.y), z(_copy.z)
{
	//
}

inline Point3F::Point3F(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
	//
}

inline Point3F::Point3F(float xyz)
	: x(xyz), y(xyz), z(xyz)
{
	//
}

inline void Point3F::set(float xyz)
{
	x = y = z = xyz;
}

inline void Point3F::set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline void Point3F::set(const Point3F& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

inline void Point3F::setMin(const Point3F& _test)
{
	x = (_test.x < x) ? _test.x : x;
	y = (_test.y < y) ? _test.y : y;
	z = (_test.z < z) ? _test.z : z;
}

inline void Point3F::setMax(const Point3F& _test)
{
	x = (_test.x > x) ? _test.x : x;
	y = (_test.y > y) ? _test.y : y;
	z = (_test.z > z) ? _test.z : z;
}

inline void Point3F::interpolate(const Point3F& _from, const Point3F& _to, float _factor)
{
	m_point3F_interpolate_C(_from, _to, _factor, *this);
}

inline void Point3F::zero()
{
	x = y = z = 0.0f;
}

inline bool Point3F::isZero() const
{
	return ((x*x) <= POINT_EPSILON) && ((y*y) <= POINT_EPSILON) && ((z*z) <= POINT_EPSILON);
}

inline bool Point3F::isUnitLength() const
{
	return (abs(1.0f - (x*x + y*y + z*z)) < POINT_EPSILON);
}

inline bool Point3F::equal(const Point3F &compare, float epsilon) const
{
	return((abs(x - compare.x) < epsilon) &&
		(abs(y - compare.y) < epsilon) &&
		(abs(z - compare.z) < epsilon));
}

inline unsigned int Point3F::getLeastComponentIndex() const
{
	unsigned int idx;

	if (abs(x) < abs(y))
	{
		if (abs(x) < abs(z))
			idx = 0;
		else
			idx = 2;
	}
	else
	{
		if (abs(y) < abs(z))
			idx = 1;
		else
			idx = 2;
	}

	return idx;
}

inline unsigned int Point3F::getGreatestComponentIndex() const
{
	unsigned int idx;

	if (abs(x) > abs(y))
	{
		if (abs(x) > abs(z))
			idx = 0;
		else
			idx = 2;
	}
	else
	{
		if (abs(y) > abs(z))
			idx = 1;
		else
			idx = 2;
	}

	return idx;
}

inline float Point3F::least() const
{
	return 0;// getMin(abs(x), getMin(abs(y), abs(z)));
}

inline float Point3F::most() const
{
	return 0;// getMax(abs(x), getMax(abs(y), abs(z)));
}

inline void Point3F::neg()
{
	x = -x;
	y = -y;
	z = -z;
}

inline void Point3F::convolve(const Point3F& c)
{
	x *= c.x;
	y *= c.y;
	z *= c.z;
}

inline void Point3F::convolveInverse(const Point3F& c)
{
	x /= c.x;
	y /= c.y;
	z /= c.z;
}

inline float Point3F::lenSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

inline float Point3F::len() const
{
	return sqrt(x*x + y*y + z*z);
}

inline void Point3F::normalize()
{
	m_point3F_normalize_C(*this);
}

inline float Point3F::magnitudeSafe() const
{
	if (isZero())
	{
		return 0.0f;
	}
	else
	{
		return len();
	}
}

inline void Point3F::normalizeSafe()
{
	float vmag = magnitudeSafe();

	if (vmag > POINT_EPSILON)
	{
		*this *= float(1.0 / vmag);
	}
}

inline void Point3F::normalize(float val)
{
	m_point3F_normalize_f_C(*this, val);
}

inline bool Point3F::operator==(const Point3F& _test) const
{
	return (x == _test.x) && (y == _test.y) && (z == _test.z);
}

inline bool Point3F::operator!=(const Point3F& _test) const
{
	return operator==(_test) == false;
}

inline Point3F Point3F::operator+(const Point3F& _add) const
{
	return Point3F(x + _add.x, y + _add.y, z + _add.z);
}

inline Point3F Point3F::operator-(const Point3F& _rSub) const
{
	return Point3F(x - _rSub.x, y - _rSub.y, z - _rSub.z);
}

inline Point3F& Point3F::operator+=(const Point3F& _add)
{
	x += _add.x;
	y += _add.y;
	z += _add.z;

	return *this;
}

inline Point3F& Point3F::operator-=(const Point3F& _rSub)
{
	x -= _rSub.x;
	y -= _rSub.y;
	z -= _rSub.z;

	return *this;
}

inline Point3F Point3F::operator*(float _mul) const
{
	return Point3F(x * _mul, y * _mul, z * _mul);
}

inline Point3F Point3F::operator/(float _div) const
{

	float inv = 1.0f / _div;

	return Point3F(x * inv, y * inv, z * inv);
}

inline Point3F& Point3F::operator*=(float _mul)
{
	x *= _mul;
	y *= _mul;
	z *= _mul;

	return *this;
}

inline Point3F& Point3F::operator/=(float _div)
{
	float inv = 1.0f / _div;
	x *= inv;
	y *= inv;
	z *= inv;

	return *this;
}

inline Point3F Point3F::operator*(const Point3F &_vec) const
{
	return Point3F(x * _vec.x, y * _vec.y, z * _vec.z);
}

inline Point3F& Point3F::operator*=(const Point3F &_vec)
{
	x *= _vec.x;
	y *= _vec.y;
	z *= _vec.z;
	return *this;
}

inline Point3F Point3F::operator/(const Point3F &_vec) const
{
	return Point3F(x / _vec.x, y / _vec.y, z / _vec.z);
}

inline Point3F& Point3F::operator/=(const Point3F &_vec)
{	
	x /= _vec.x;
	y /= _vec.y;
	z /= _vec.z;
	return *this;
}

inline Point3F Point3F::operator-() const
{
	return Point3F(-x, -y, -z);
}



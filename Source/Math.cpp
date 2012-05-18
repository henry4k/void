#include <cmath>
#include "Math.hpp"
using namespace std;


#define VEC_OP_VEC(OP) \
	Vec3f r; \
	r.x = a.x OP b.x; \
	r.y = a.y OP b.y; \
	r.z = a.z OP b.z; \
	return r;

Vec3f Add( const Vec3f& a, const Vec3f& b )
{
	VEC_OP_VEC(+)
}

Vec3f Sub( const Vec3f& a, const Vec3f& b )
{
	VEC_OP_VEC(-)
}

Vec3f Mul( const Vec3f& a, const Vec3f& b )
{
	VEC_OP_VEC(*)
}

Vec3f Div( const Vec3f& a, const Vec3f& b )
{
	VEC_OP_VEC(/)
}


#define VEC_OP_FACTOR(OP) \
	Vec3f r; \
	r.x = a.x OP b; \
	r.y = a.y OP b; \
	r.z = a.z OP b; \
	return r;

Vec3f Add( const Vec3f& a, float b )
{
	VEC_OP_FACTOR(+)
}

Vec3f Sub( const Vec3f& a, float b )
{
	VEC_OP_FACTOR(-)
}

Vec3f Mul( const Vec3f& a, float b )
{
	VEC_OP_FACTOR(*)
}

Vec3f Div( const Vec3f& a, float b )
{
	VEC_OP_FACTOR(/)
}




float LengthSqrt( const Vec3f& vec )
{
	return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
}

float Length( const Vec3f& vec )
{
	return sqrt(LengthSqrt(vec));
}

Vec3f Normalized( const Vec3f& vec )
{
	float length = Length(vec);
	Vec3f r;
	r.x = vec.x / length;
	r.y = vec.y / length;
	r.z = vec.z / length;
	return r;
}

float Dot( const Vec3f& a, const Vec3f& b )
{
	return
		a.x * b.x +
		a.y * b.y +
		a.z * b.z;
}

Vec3f Cross( const Vec3f& a, const Vec3f& b )
{
	Vec3f r;
	r.x = a.y*b.z - a.z*b.y;
	r.y = a.z*b.x - a.x*b.z;
	r.z = a.x*b.y - a.y*b.x;
	return r;
}
#ifndef MATH_HPP
#define MATH_HPP

struct Vec2f { float x, y; };
struct Vec3f { float x, y, z; };
struct Vec4f { float x, y, z, w; };

Vec3f Add( const Vec3f& a, const Vec3f& b );
Vec3f Sub( const Vec3f& a, const Vec3f& b );
Vec3f Mul( const Vec3f& a, const Vec3f& b );
Vec3f Div( const Vec3f& a, const Vec3f& b );

Vec3f Add( const Vec3f& a, float b );
Vec3f Sub( const Vec3f& a, float b );
Vec3f Mul( const Vec3f& a, float b );
Vec3f Div( const Vec3f& a, float b );

float Length( const Vec3f& vec );
Vec3f Normalized( const Vec3f& vec );

float Dot( const Vec3f& a, const Vec3f& b );
Vec3f Cross( const Vec3f& a, const Vec3f& b );

#endif
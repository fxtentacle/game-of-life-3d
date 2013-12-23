/***************************************************************************
* Vec3.h                                                                   *
*                                                                          *
* Vec3 is a trivial encapsulation of 3D floating-point coordinates.        *
* It has all of the obvious operators defined as inline functions.         *
*                                                                          *
* History:                                                                 *
*   10/21/2003  Updated for intro graphics class.                          *
*   04/01/2003  Initial coding.                                            *
*                                                                          *
***************************************************************************/
#ifndef __VEC3_INCLUDED__
#define __VEC3_INCLUDED__

#include <iostream>
#include <cmath>

struct Vec3 {
    inline Vec3() { x = 0; y = 0; z = 0; }
    inline Vec3(float a) { x = a; y = a; z = a; }
    inline Vec3( float a, float b, float c ) { x = a; y = b; z = c; }
    inline Vec3( float a, float b, float c, float d ) { x = a; y = b; z = c; }
    inline Vec3( const float *a ) { x = *a++; y = *a++; z = *a; }
    float x;
    float y;
    float z;
    
    inline void copyTo(float* tg) const {
        tg[0] =x;tg[1]=y;tg[2]=z;
    }
    

    };


inline float LengthSquared( const Vec3 &A )
    {
    return A.x * A.x + A.y * A.y + A.z * A.z;
    }

inline float Length( const Vec3 &A )
    {
    return sqrt( LengthSquared( A ) );
    }

inline Vec3 operator+( const Vec3 &A, const Vec3 &B )
    {
    return Vec3( A.x + B.x, A.y + B.y, A.z + B.z );
    }

inline Vec3 operator-( const Vec3 &A, const Vec3 &B )
    {
    return Vec3( A.x - B.x, A.y - B.y, A.z - B.z );
    }

inline Vec3 operator-( const Vec3 &A )  // Unary minus.
    {
    return Vec3( -A.x, -A.y, -A.z );
    }

inline Vec3 operator*( float a, const Vec3 &A )
    {
    return Vec3( a * A.x, a * A.y, a * A.z );
    }

inline Vec3 operator*( const Vec3 &A, float a )
    {
    return Vec3( a * A.x, a * A.y, a * A.z );
    }

inline Vec3 operator/( const Vec3 &A, float c )
    {
    return Vec3( A.x / c, A.y / c, A.z / c );
    }

inline float operator*( const Vec3 &A, const Vec3 &B )  // Dot product.
    {
    return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
    }

inline Vec3 operator^( const Vec3 &A, const Vec3 &B ) // Cross product.
    {
    return Vec3( 
        A.y * B.z - A.z * B.y,
        A.z * B.x - A.x * B.z,
        A.x * B.y - A.y * B.x
        );
    }

inline Vec3 &operator+=( Vec3 &A, const Vec3 &B )
    {
    A.x += B.x;
    A.y += B.y;
    A.z += B.z;
    return A;
    }

inline Vec3 &operator-=( Vec3 &A, const Vec3 &B )
    {
    A.x -= B.x;
    A.y -= B.y;
    A.z -= B.z;
    return A;
    }

inline Vec3 &operator*=( Vec3 &A, float a )
    {
    A.x *= a;
    A.y *= a;
    A.z *= a;
    return A;
    }

inline Vec3& operator/=( Vec3 &A, float a )
    {
    A.x /= a;
    A.y /= a;
    A.z /= a;
    return A;
    }

inline Vec3 Unit( const Vec3 &A ) // Return a normalized vector (if it is non-zero).
    {
    float d = LengthSquared( A );
    return d > 0.0 ? A / sqrt(d) : Vec3(0,0,0);
    }

inline Vec3 OrthogonalTo( const Vec3 &A )  // Return a non-zero vector orthogonal to A.
    {
    if( A.x == 0 ) return Vec3( 1, 0, 0 );
    return Vec3( A.y, -A.x, 0 );
    }

inline std::ostream &operator<<( std::ostream &out, const Vec3 &A )
    {
    out << "(" << A.x << ", " << A.y << ", " << A.z << ") ";
    return out;
    }

#endif



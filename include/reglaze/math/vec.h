#ifndef RGLZ_MATH_VEC_H
#define RGLZ_MATH_VEC_H

#include "base.h"
#include <cstdint>

namespace rglz {
	struct Vec2 {
		inline Vec2 operator+ (const Vec2& vec) const {
			return Vec2{x + vec.x, y + vec.y};
		}

		inline Vec2& operator+= (const Vec2& vec) {
			x += vec.x;
			y += vec.y;

			return *this;
		}

		inline Vec2 operator- () const {
			return Vec2{-x, -y};
		}

		inline Vec2 operator- (const Vec2& vec) const {
			return Vec2{x - vec.x, y - vec.y};
		}

		inline Vec2& operator-= (const Vec2& vec) {
			x -= vec.x;
			y -= vec.y;

			return *this;
		}

		inline Vec2 operator* (float scalar) const {
			return Vec2{x * scalar, y * scalar};
		}

		inline Vec2& operator*= (float scalar) {
			x *= scalar;
			y *= scalar;

			return *this;
		}

		inline Vec2 operator/ (float scalar) const {
			return Vec2{x / scalar, y / scalar};
		}

		inline Vec2& operator/= (float scalar) {
			x /= scalar;
			y /= scalar;

			return *this;
		}

		inline static float dot (const Vec2& vec_a, const Vec2& vec_b) {
			return vec_a.x * vec_b.x + vec_a.y * vec_b.y;
		}


		float x;	
		float y;	
	};

	struct Vec3 {
		inline Vec3 operator+ (const Vec3& vec) const {
			return Vec3{x + vec.x, y + vec.y, z + vec.z};
		}

		inline Vec3& operator+= (const Vec3& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;

			return *this;
		}

		inline Vec3 operator- () const {
			return Vec3{-x, -y, -z};
		}

		inline Vec3 operator- (const Vec3& vec) const {
			return Vec3{x - vec.x, y - vec.y, z - vec.z};
		}

		inline Vec3& operator-= (const Vec3& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;

			return *this;
		}

		inline Vec3 operator* (float scalar) const {
			return Vec3{x * scalar, y * scalar, z * scalar};
		}

		inline Vec3& operator*= (float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;

			return *this;
		}

		inline Vec3 operator/ (float scalar) const {
			return Vec3{x / scalar, y / scalar, z / scalar};
		}

		inline Vec3& operator/= (float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;

			return *this;
		}

		inline static float dot (const Vec3& vec_a, const Vec3& vec_b) {
			return vec_a.x * vec_b.x + vec_a.y * vec_b.y + vec_a.z * vec_b.z;
		}

		float x;	
		float y;	
		float z;
	};

	inline Vec2 operator*(float scalar, const Vec2& vec)
	{
		return vec * scalar;
	}

	inline Vec3 operator*(float scalar, const Vec3& vec)
	{
		return vec * scalar;
	}
}


#endif	// RGLZ_MATH_VEC_H

#ifndef RGLZ_MATH_VEC_H
#define RGLZ_MATH_VEC_H

#include "base.h"
#include <array>
#include <cstdint>

namespace rglz {
	union Vec2 {
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

		inline float& operator[] (uint8_t i) {
			RGLZ_ASSERT(i < 2);
			return data[i];
		}

		inline const float& operator[] (uint8_t i) const {
			RGLZ_ASSERT(i < 2);
			return data[i];
		}

		struct  {
			float x;	
			float y;	
		};
		struct {
			std::array<float, 2> data;
		};
	};

	union Vec3 {
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

		inline float& operator[] (uint8_t i) {
			RGLZ_ASSERT(i < 3);
			return data[i];
		}

		inline const float& operator[] (uint8_t i) const {
			RGLZ_ASSERT(i < 3);
			return data[i];
		}

		struct {
			float x;	
			float y;	
			float z;
		};
		struct {
			std::array<float, 3> data;
		};
	};

	union Vec4 {
		inline Vec4 operator+ (const Vec4& vec) const {
			return Vec4{x + vec.x, y + vec.y, z + vec.z, w + vec.w};
		}

		inline Vec4& operator+= (const Vec4& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			w += vec.w;

			return *this;
		}

		inline Vec4 operator- () const {
			return Vec4{-x, -y, -z, -w};
		}

		inline Vec4 operator- (const Vec4& vec) const {
			return Vec4{x - vec.x, y - vec.y, z - vec.z, w - vec.w};
		}

		inline Vec4& operator-= (const Vec4& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			w -= vec.w;

			return *this;
		}

		inline Vec4 operator* (float scalar) const {
			return Vec4{x * scalar, y * scalar, z * scalar, w * scalar};
		}

		inline Vec4& operator*= (float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;

			return *this;
		}

		inline Vec4 operator/ (float scalar) const {
			return Vec4{x / scalar, y / scalar, z / scalar, w / scalar};
		}

		inline Vec4& operator/= (float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
			y /= scalar;

			return *this;
		}

		inline static float dot (const Vec4& vec_a, const Vec4& vec_b) {
			return vec_a.x * vec_b.x + vec_a.y * vec_b.y + vec_a.z * vec_b.z + vec_a.w * vec_b.w;
		}

		struct {
			float x;	
			float y;	
			float z;
			float w;
		};

		inline float& operator[] (uint8_t i) {
			RGLZ_ASSERT(i < 4);
			return data[i];
		}

		inline const float& operator[] (uint8_t i) const {
			RGLZ_ASSERT(i < 4);
			return data[i];
		}

		struct {
			std::array<float, 4> data;
		};
	};

	inline Vec2 operator*(float scalar, const Vec2& vec)
	{
		return vec * scalar;
	}

	inline Vec3 operator*(float scalar, const Vec3& vec)
	{
		return vec * scalar;
	}

	inline Vec4 operator*(float scalar, const Vec4& vec) {
		return vec * scalar;
	}
}


#endif	// RGLZ_MATH_VEC_H
